#include "SD_Card.h"
#include "Display_ST7789.h"
#include "LCD_Image.h"
#include "LVGL_Video.h"
#include "WS2812B.h"
#include "LCD_Text.h"

// 全局变量：标记SD卡是否检测到
bool sdCardDetected = false;

void setup()
{
  Flash_test();  
  LCD_Init();    
  Set_Backlight(100);
  
  // 初始化WS2812B
  WS2812B_Init();
  
  // RGB测试序列
  // 1. 屏幕显示红色，WS2812B显示红灯1秒
  LCD_FillScreen(RGB565(255, 0, 0));  // 红色
  WS2812B_SetColor(255, 0, 0);        // WS2812B红色
  delay(1000);
  
  // 2. 屏幕显示绿色，WS2812B显示绿灯1秒
  LCD_FillScreen(RGB565(0, 255, 0));  // 绿色
  WS2812B_SetColor(0, 255, 0);        // WS2812B绿色
  delay(1000);
  
  // 3. 屏幕显示蓝色，WS2812B显示蓝灯1秒
  LCD_FillScreen(RGB565(0, 0, 255));  // 蓝色
  WS2812B_SetColor(0, 0, 255);        // WS2812B蓝色
  delay(1000);
  
  // 关闭WS2812B
  WS2812B_Off();
  
  // 初始化SD卡
  SD_Init();
  
  // 检查SD卡是否检测到
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    // 没有检测到SD卡，显示错误信息
    LCD_ShowError("SD card not detected");
    sdCardDetected = false;
    // 保持显示错误信息，不继续执行
    return;
  }
  
  sdCardDetected = true;
  
  // 检查是否有视频文件，如果有则初始化视频播放，否则显示图片
  if (Video_CheckFiles()) {
    // 有视频文件，初始化视频播放
    Video_Init();
  } else {
    // 没有视频文件，显示图片
    Display_Image("/",".png", 0);
    pinMode(BOOT_KEY_PIN, INPUT);   
  }
}

void loop()
{
  // 如果SD卡未检测到，不执行任何操作
  if (!sdCardDetected) {
    delay(1000);
    return;
  }
  
  // 如果视频系统已初始化，播放视频；否则显示图片
  if (Video_IsInitialized()) {
    // 播放视频
    Video_Play_Loop();
  } else {
    // 显示图片
    Image_Next_Loop("/",".png",300);
    delay(5);
  }
}
