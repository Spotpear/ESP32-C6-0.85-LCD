#pragma once

#include <Arduino.h>
#include <FastLED.h>

// WS2812B配置
#define WS2812B_PIN       8   // WS2812B数据引脚，根据实际硬件修改
#define NUM_LEDS          1    // LED数量，根据实际硬件修改
#define BRIGHTNESS         50  // 亮度 (0-255)

// WS2812B初始化
void WS2812B_Init(void);

// 设置WS2812B颜色 (R, G, B)
void WS2812B_SetColor(uint8_t r, uint8_t g, uint8_t b);

// 关闭WS2812B
void WS2812B_Off(void);

