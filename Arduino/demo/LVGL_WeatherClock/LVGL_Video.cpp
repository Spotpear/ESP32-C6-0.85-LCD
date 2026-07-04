/***
 * SD卡视频播放功能
 * 参考main文件夹实现，使用LVGL_Image的显示屏驱动和引脚配置
 * 
 * Required libraries:
 * JPEGDEC: https://github.com/bitbank2/JPEGDEC.git
 */

#include "LVGL_Video.h"
#include "Display_ST7789.h"
#include "SD_Card.h"
#include "MjpegClass.h"
#include <FS.h>

#include <lvgl.h>
#include "generated/ui_fonts/lv_customer_font_Acme_Regular_32.c"

#include "LCD_Image.h"  // 包含BOOT_KEY_PIN定义

// 在现有全局变量下方添加
volatile bool videoModeActive = false;
static bool request_exit_video = false;  // 请求退出视频模式
// 视频文件列表
const char* videoFiles[] = {
  "/video1.mjpeg", 
  "/video2.mjpeg", 
  "/video3.mjpeg"
};
const int TOTAL_VIDEOS = sizeof(videoFiles) / sizeof(videoFiles[0]);

#define FPS 20 // 目标帧率
#define PRELOAD_THRESHOLD 0.8f // 预加载阈值
// 减小MJPEG缓冲区大小，不需要整个屏幕大小，只需要足够存储一个JPEG帧
// 使用较小的缓冲区，ESP32-C6内存有限
#define MJPEG_BUFFER_SIZE (32 * 1024) // 32KB缓冲区

// 全局变量
static uint8_t currentVideo = 0;  // 当前视频索引
static uint8_t nextVideoIndex = 0; // 下一个视频索引
static bool isFirstPlay = true;   // 标记是否是第一次播放
static bool manualSwitchRequested = false; // 手动切换请求标志
static bool videoEnded = false;   // 视频自然播放完毕标志

// 按键相关变量
static volatile bool buttonStateChanged = false; // 按键状态变化标志（ISR设置）
static bool buttonIsCurrentlyPressed = false;     // 当前按键物理状态
const unsigned long buttonDebounceTime = 50; // 按键消抖时间(ms)

// 动态分配缓冲区，避免静态分配过大导致堆栈溢出
static uint8_t* mjpeg_buf = nullptr; 

// 文件对象
static File vFile; // 当前视频文件
static File nextVFile; // 下一个视频文件（预加载）

static MjpegClass mjpeg;

// 视频系统初始化标志
static bool video_initialized = false; // 视频系统是否已初始化
static bool sdCardDetected = false;    // SD卡是否被检测到
static bool noSdCardMode = false;      // 是否处于无SD卡模式

// 绘制回调函数 - 直接显示MCU块，减少SPI传输次数
static int drawMCU(JPEGDRAW *pDraw) {
  uint16_t* pixels = (uint16_t*)pDraw->pPixels;
  int x = pDraw->x;
  int y = pDraw->y;
  int width = pDraw->iWidth;
  int height = pDraw->iHeight;
  
  if (x < 0) {
    width += x;
    x = 0;
  }
  if (y < 0) {
    height += y;
    y = 0;
  }
  if (x + width > LCD_WIDTH) {
    width = LCD_WIDTH - x;
  }
  if (y + height > LCD_HEIGHT) {
    height = LCD_HEIGHT - y;
  }
  
  if (width <= 0 || height <= 0 || x >= LCD_WIDTH || y >= LCD_HEIGHT) {
    return 1;
  }
  
  uint16_t x_end = x + width - 1;
  uint16_t y_end = y + height - 1;
  
  if (x_end >= LCD_WIDTH) x_end = LCD_WIDTH - 1;
  if (y_end >= LCD_HEIGHT) y_end = LCD_HEIGHT - 1;
  
  LCD_addWindow(x, y, x_end, y_end, pixels);
  
  return 1;
}

// 显示完整的帧（不再需要，因为drawMCU直接显示）
static void displayFrame() {
  // drawMCU已经直接显示，这里不需要额外操作
}

// 内存监控
static void printMemoryStats() {
  Serial.printf("Heap: Free=%d, Min=%d, MaxAlloc=%d\n", 
                ESP.getFreeHeap(), 
                ESP.getMinFreeHeap(),
                ESP.getMaxAllocHeap());
}

// 释放当前视频资源
static void releaseCurrentVideo() {
  if (vFile) {
    vFile.close();
    vFile = File(); // 重置文件对象
  }
}

// 释放下一个视频资源
static void releaseNextVideo() {
  if (nextVFile) {
    nextVFile.close();
    nextVFile = File();
  }
}

// BOOT按钮中断服务程序
void IRAM_ATTR buttonISR() {
  // 简单的消抖处理
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  
  if (interrupt_time - last_interrupt_time > buttonDebounceTime) {
    buttonStateChanged = true;
    buttonIsCurrentlyPressed = true;
  }
  last_interrupt_time = interrupt_time;
}

// 预加载下一个视频
static bool preloadNextVideo() {
  // 确定下一个视频索引 (0-2循环)
  nextVideoIndex = (currentVideo + 1) % TOTAL_VIDEOS;
  
  // 释放之前的预加载
  releaseNextVideo();
  
  const char *nextFilename = videoFiles[nextVideoIndex];
  
  // 尝试打开下一个视频文件
  nextVFile = SD.open(nextFilename);
  if (!nextVFile || nextVFile.isDirectory()) {
    Serial.printf("Preload failed for: %s\n", nextFilename);
    return false;
  }
  
  Serial.printf("Preloaded next video: %s\n", nextFilename);
  return true;
}

// 切换到下一个视频（调用者已释放当前视频）
static void switchToNextVideo() {
  if (request_exit_video) {
        return;
    }
  
  // 切换到预加载的视频
  currentVideo = nextVideoIndex;
  vFile = nextVFile;
  nextVFile = File(); // 重置预加载文件
  
  // 预加载下一个视频
  preloadNextVideo();
  
  // 重置手动切换标志
  manualSwitchRequested = false;
}

// 播放视频函数
static void playVideo() {
  const char *filename = videoFiles[currentVideo];
  
  // 内存检查
  printMemoryStats();
  
  // 尝试打开文件
  if (!vFile) {
    vFile = SD.open(filename);
  }
  
  if (!vFile || vFile.isDirectory()) {
    Serial.printf("FATAL: Failed to open %s\n", filename);
    
    // 切换到下一个视频
    currentVideo = (currentVideo + 1) % TOTAL_VIDEOS;
    return;
  }

  Serial.printf("Playing: %s (Size: %d bytes)\n", filename, vFile.size());
  
  // 初始化显示 - 仅第一次播放时执行
  if (isFirstPlay) {
    // 清屏 - 创建黑色缓冲区
    static uint16_t black_buffer[LCD_WIDTH];
    for (int i = 0; i < LCD_WIDTH; i++) {
      black_buffer[i] = 0x0000;
    }
    for (int y = 0; y < LCD_HEIGHT; y++) {
      LCD_addWindow(0, y, LCD_WIDTH - 1, y, black_buffer);
    }
    isFirstPlay = false;
  }
  
  // 检查缓冲区是否已分配
  if (!mjpeg_buf) {
    Serial.println("MJPEG buffer not allocated!");
    releaseCurrentVideo();
    currentVideo = (currentVideo + 1) % TOTAL_VIDEOS;
    return;
  }
  
  // 使用动态分配的缓冲区
  if (!mjpeg.setup(&vFile, mjpeg_buf, drawMCU, true, 0, 0, LCD_WIDTH, LCD_HEIGHT)) {
    Serial.println("MJPEG setup failed");
    releaseCurrentVideo();
    
    // 切换到下一个视频
    currentVideo = (currentVideo + 1) % TOTAL_VIDEOS;
    return;
  }

  // 初始化计时器
  unsigned long start_ms = millis();
  unsigned long next_frame_ms = start_ms;
  
  // 获取文件总大小用于进度计算
  size_t totalSize = vFile.size();
  size_t currentPosition = 0;
  
  // 按键计时
  unsigned long buttonPressStartTime = 0;
  bool buttonProcessing = false;

  while (vFile.available()) {
    unsigned long curr_ms = millis();

    if (curr_ms >= next_frame_ms) {
      if (!mjpeg.readMjpegBuf()) {
        break;
      }

      currentPosition = vFile.position();

      if (currentPosition > 0 && totalSize > 0 &&
          static_cast<float>(currentPosition) / totalSize > PRELOAD_THRESHOLD) {
        if (!nextVFile) {
          preloadNextVideo();
        }
      }

      if (!mjpeg.drawJpg()) {
        Serial.println("Draw JPG failed");
        break;
      }

      next_frame_ms += (1000 / FPS);
    } else {
      delay(1);
    }

    // 检测按键按下状态
    if (buttonIsCurrentlyPressed) {
      if (!buttonProcessing) {
        buttonProcessing = true;
        buttonPressStartTime = millis();
      } else if (digitalRead(BOOT_KEY_PIN) == HIGH) {
        // 按键已释放
        unsigned long pressDuration = millis() - buttonPressStartTime;
        buttonIsCurrentlyPressed = false;
        buttonStateChanged = false;
        buttonProcessing = false;
        
        if (pressDuration < 500) {
          // 短按：切换下一个视频
          videoEnded = false;
          if (currentVideo == TOTAL_VIDEOS - 1) {
            request_exit_video = true;
            Serial.println("Last video, requesting exit to weather clock");
            break;
          } else {
            manualSwitchRequested = true;
            Serial.println("Short press! Requesting manual switch");
            break;
          }
        }
        // 如果 500ms~800ms 不做处理，继续等长按
      } else if (millis() - buttonPressStartTime > 800) {
        // 长按：退出到天气时钟
        request_exit_video = true;
        videoEnded = false;
        buttonIsCurrentlyPressed = false;
        buttonStateChanged = false;
        buttonProcessing = false;
        Serial.println("Long press! Exiting to weather clock");
        break;
      }
    }
  }
  Serial.println("Playback ended");
  
  // 释放当前视频资源
  releaseCurrentVideo();
  
  // 检查是否需要切换视频
  if (manualSwitchRequested) {
    videoEnded = false;
    if (nextVFile) {
      switchToNextVideo();  // 转移预加载文件到vFile
    } else {
      currentVideo = (currentVideo + 1) % TOTAL_VIDEOS;
    }
  } else {
    videoEnded = true;
  }
}

// 检查SD卡状态
static bool CheckSDCardStatus(void) {
  return SD.begin();
}


// 显示无SD卡提示
static void DisplayNoSDCardMessage(void) {
  // 清屏为黑色
  static uint16_t black_buffer[LCD_WIDTH];
  for (int i = 0; i < LCD_WIDTH; i++) {
    black_buffer[i] = 0x0000;
  }
  for (int y = 0; y < LCD_HEIGHT; y++) {
    LCD_addWindow(0, y, LCD_WIDTH - 1, y, black_buffer);
  }
  
  // 使用LVGL显示文字
  lv_obj_clean(lv_scr_act());
  
  lv_obj_t* label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "No SD Card!");
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);
  
  lv_obj_t* sublabel = lv_label_create(lv_scr_act());
  lv_label_set_text(sublabel, "BOOT to exit");
  lv_obj_set_style_text_color(sublabel, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(sublabel, &lv_font_montserrat_14, 0);
  lv_obj_align(sublabel, LV_ALIGN_CENTER, 0, 20);
  
  lv_task_handler();
  delay(100);
}

// 检查视频文件是否存在
bool Video_CheckFiles(void) {
  bool foundAny = false;
  
  for (int i = 0; i < TOTAL_VIDEOS; i++) {
    if (SD.exists(videoFiles[i])) {
      foundAny = true;
      File testFile = SD.open(videoFiles[i]);
      if (testFile) {
        Serial.printf("%s found, Size: %d bytes\n", videoFiles[i], testFile.size());
        testFile.close();
      }
    } else {
      Serial.printf("%s not found!\n", videoFiles[i]);
    }
  }
  
  return foundAny;
}

// 视频播放初始化（一次性setup中调用，只检查文件）
void Video_Init(void) {
  Serial.println("Video system initializing...");
  printMemoryStats();

  // 检查所有视频文件
  Serial.println("Checking video files...");
  if (!Video_CheckFiles()) {
    Serial.println("No video files found!");
    return;
  }

  video_initialized = true;
  Serial.println("Video system ready");
  printMemoryStats();
}

// 切换到视频模式（打开第一个视频，分配缓冲区，注册按键中断）
void Video_Start(void) {

   // 检查SD卡状态
  sdCardDetected = CheckSDCardStatus();
  
  if (!sdCardDetected) {
    // 没有SD卡，显示提示信息
    Serial.println("No SD card detected, displaying warning message");
    noSdCardMode = true;
    request_exit_video = false;
    videoModeActive = true;
    
    // 显示无SD卡提示
    DisplayNoSDCardMessage();
    
    // 初始化BOOT按钮用于退出
    pinMode(BOOT_KEY_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BOOT_KEY_PIN), buttonISR, FALLING);
    Serial.println("Button initialized for exit from no-SD mode");
    return;
  }
  
  // 有SD卡，继续正常视频模式流程
  noSdCardMode = false;

  if (!video_initialized) {
    Serial.println("Video system not initialized!");
    return;
  }
  
  request_exit_video = false;
  videoModeActive = true;
  currentVideo = 0;
  isFirstPlay = true;
  manualSwitchRequested = false;
  videoEnded = false;

  // 分配MJPEG缓冲区（在Video_Start中分配，Video_Init时不分配）
  if (!mjpeg_buf) {
    mjpeg_buf = (uint8_t*)malloc(MJPEG_BUFFER_SIZE);
    if (!mjpeg_buf) {
      Serial.println("Failed to allocate MJPEG buffer!");
      Serial.printf("Available heap: %d bytes\n", ESP.getFreeHeap());
      return;
    }
    Serial.printf("MJPEG buffer allocated: %d bytes\n", MJPEG_BUFFER_SIZE);
  }
  
  // 初始化BOOT按钮
  pinMode(BOOT_KEY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BOOT_KEY_PIN), buttonISR, FALLING);
  Serial.printf("Button initialized on pin %d (INPUT_PULLUP, FALLING interrupt)\n", BOOT_KEY_PIN);
  
  // 打开第一个视频文件
  vFile = SD.open(videoFiles[currentVideo]);
  if (!vFile || vFile.isDirectory()) {
    Serial.printf("Failed to open first video: %s\n", videoFiles[currentVideo]);
    currentVideo = (currentVideo + 1) % TOTAL_VIDEOS;
    vFile = SD.open(videoFiles[currentVideo]);
    if (!vFile || vFile.isDirectory()) {
      Serial.println("Failed to open any video.");
      return;
    }
  }
  Serial.printf("First video opened: %s\n", videoFiles[currentVideo]);
  
  // 预加载下一个视频
  preloadNextVideo();
  
  Serial.println("Video mode started");
  printMemoryStats();
}

// 无SD卡模式循环处理
static void NoSdCardMode_Loop(void) {
  // 处理按键，允许短按或长按退出
  if (buttonIsCurrentlyPressed) {
    unsigned long pressStartTime = millis();
    while (digitalRead(BOOT_KEY_PIN) == LOW && millis() - pressStartTime < 2000) {
      delay(10);
    }
    unsigned long pressDuration = millis() - pressStartTime;
    buttonIsCurrentlyPressed = false;
    buttonStateChanged = false;
    
    // 无论短按还是长按，都请求退出
    request_exit_video = true;
    Serial.printf("Button pressed in no-SD mode (%lu ms), exiting to weather clock\n", pressDuration);
  }
  
  // 刷新LVGL任务
  lv_task_handler();
  delay(10);
}

// 退出视频模式（释放按键中断）
void Video_Stop(void) {
  videoModeActive = false;
  detachInterrupt(digitalPinToInterrupt(BOOT_KEY_PIN));
  releaseCurrentVideo();
  releaseNextVideo();
  mjpeg.cleanup();
  Serial.println("Video mode stopped");
}

// 检查视频系统是否已初始化
bool Video_IsInitialized(void) {
  return video_initialized;
}

// 视频播放循环（在loop中调用）
void Video_Play_Loop(void) {

  // 无SD卡模式处理
    if (noSdCardMode) {
        NoSdCardMode_Loop();
        return;
    }
    
    if (!video_initialized) {
        return;
    }
    
    // 视频自然播放完毕，等待按键而不重播
    if (videoEnded) {
        if (buttonIsCurrentlyPressed) {
            unsigned long pressStartTime = millis();
            while (digitalRead(BOOT_KEY_PIN) == LOW && millis() - pressStartTime < 2000) {
                delay(10);
            }
            unsigned long pressDuration = millis() - pressStartTime;
            buttonIsCurrentlyPressed = false;
            buttonStateChanged = false;
            videoEnded = false;
            
            if (pressDuration > 800) {
                // 长按：退出到天气时钟
                request_exit_video = true;
                Serial.println("Last video ended, long press - exiting to weather clock");
            } else {
                // 短按：切换到下一个视频
                if (currentVideo == TOTAL_VIDEOS - 1) {
                    request_exit_video = true;
                    Serial.println("Last video ended, requesting exit to weather clock");
                } else {
                    currentVideo = (currentVideo + 1) % TOTAL_VIDEOS;
                    Serial.printf("Short press, switching to next video: %d\n", currentVideo + 1);
                }
            }
        }
        delay(10);
        return;
    }
    
    // 确保文件已打开
    if (!vFile) {
        const char *filename = videoFiles[currentVideo];
        vFile = SD.open(filename);
        if (!vFile || vFile.isDirectory()) {
            Serial.printf("Failed to open video: %s\n", filename);
            currentVideo = (currentVideo + 1) % TOTAL_VIDEOS;
            return;
        }
        releaseNextVideo();
        preloadNextVideo();
    }
    
    // 播放当前视频
    playVideo();
    
    // 如果请求退出视频模式，清理并返回
    if (request_exit_video) {
        return;
    }
    
    // 手动切换时，视频已在playVideo中切换完成
    // 视频自然结束时，videoEnded已设为true，下次循环会等待
    
    delay(10);
}
// 检查是否需要退出视频模式
bool Video_ShouldExit(void) {
    return request_exit_video;
}

// 重置退出标志（切换回天气时钟后调用）
void Video_ResetExitFlag(void) {
    request_exit_video = false;
}
