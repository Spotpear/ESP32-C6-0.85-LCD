#include "WS2812B.h"

CRGB leds[NUM_LEDS];

void WS2812B_Init(void) {
  FastLED.addLeds<WS2812B, WS2812B_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  WS2812B_Off();  // 初始化为关闭状态
}

void WS2812B_SetColor(uint8_t r, uint8_t g, uint8_t b) {
  leds[0] = CRGB(r, g, b);
  FastLED.show();
}

void WS2812B_Off(void) {
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
}

