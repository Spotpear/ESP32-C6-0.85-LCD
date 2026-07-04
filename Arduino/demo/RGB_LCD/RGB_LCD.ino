
#include "Display_ST7789.h"
#include "WS2812B.h"



void setup()
{
  LCD_Init();    
  Set_Backlight(100);
  WS2812B_Init();
}
void loop()
{
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
}
