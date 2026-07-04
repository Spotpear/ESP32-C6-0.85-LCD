#include "SD_Card.h"
#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "WeatherClock.h"
#include "WiFi.h"
#include "LVGL_Video.h"
#include "LCD_Image.h"
#include "WS2812B.h"

#define MODE_WEATHER 0
#define MODE_GIF     1
#define MODE_VIDEO   2

#define LONG_PRESS_MS 800  // 长按阈值（毫秒）

static uint8_t currentMode = MODE_WEATHER;

static unsigned long lastButtonCheck = 0;
static unsigned long buttonPressStart = 0;
static bool buttonWasPressed = false;
uint8_t num[]={0,1,2,16,17,18,19,20,21,22,23};//引脚定义

void setup()
{       
  Serial.begin(115200);
  delay(100);
  Serial.println("System starting...");

  for(uint8_t i=0; i<sizeof(num)/sizeof(num[0]); i++) {
    printf("Toggle pin %d\n", num[i]);
    pinMode(num[i], OUTPUT);     
    delay(30);       
    digitalWrite(num[i], LOW);
    delay(30);  
    
  }
  
  LCD_Init();
  Set_Backlight(0);
  WS2812B_Init();
  
  LCD_FillScreen(0xFFFF);
  for (int i = 0; i <= 50; i++) {
    Set_Backlight(i);
    delay(50);
  }

  Set_Backlight(100);
  LCD_FillScreen(0xF800);
  WS2812B_SetColor(255, 0, 0);
  delay(500);
  LCD_FillScreen(0x07E0);
  WS2812B_SetColor(0, 255, 0);
  delay(500);
  LCD_FillScreen(0x001F);
  WS2812B_SetColor(0, 0, 255);
  delay(500);
  WS2812B_Off();
  
  SD_Init();
  
  SPI.begin(EXAMPLE_PIN_NUM_SCLK, EXAMPLE_PIN_NUM_MISO, EXAMPLE_PIN_NUM_MOSI);
  delay(50);
  
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifi_ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 30000) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("WiFi connection timeout, continuing anyway...");
  }
  
  Video_Init();
  
  Lvgl_Init();
  
  Set_Backlight(100);
  WeatherClock_Init();
  
  Serial.println("Setup complete!");
}

void loop()
{
  switch (currentMode) {
    case MODE_WEATHER:
      Timer_Loop();
      WeatherClock_Loop();
      
      // 长按检测：切换到GIF模式
      if (millis() - lastButtonCheck > 20) {
        lastButtonCheck = millis();
        if (digitalRead(BOOT_KEY_PIN) == LOW) {
          if (!buttonWasPressed) {
            buttonWasPressed = true;
            buttonPressStart = millis();
          } else if (millis() - buttonPressStart > LONG_PRESS_MS) {
            Serial.println("Long press: switching to GIF mode...");
             LCD_FillScreen(0x0000);
              delay(200);  
            currentMode = MODE_GIF;
            WeatherClock_ShowGifOnly();
            buttonWasPressed = false;
          }
        } else {
          buttonWasPressed = false;
        }
      }
      break;
      
    case MODE_GIF:
      Timer_Loop();
      
      // 长按检测：切换到视频模式
      if (millis() - lastButtonCheck > 20) {
        lastButtonCheck = millis();
        if (digitalRead(BOOT_KEY_PIN) == LOW) {
          if (!buttonWasPressed) {
            buttonWasPressed = true;
            buttonPressStart = millis();
          } else if (millis() - buttonPressStart > LONG_PRESS_MS) {
            Serial.println("Long press: switching to video mode...");
            currentMode = MODE_VIDEO;
            Video_Start();
            buttonWasPressed = false;
          }
        } else {
          buttonWasPressed = false;
        }
      }
      break;
      
    case MODE_VIDEO:
      Video_Play_Loop();
      
      if (Video_ShouldExit()) {
        Serial.println("Exiting video mode, returning to weather clock...");
        Video_ResetExitFlag();
        Video_Stop();
        currentMode = MODE_WEATHER;
        WeatherClock_Init();
      }
      break;
  }
  
  delay(1);
}