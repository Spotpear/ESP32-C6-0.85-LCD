#include "LCD_Text.h"
#include <cstring>

// 简单的5x7字体数据（仅包含需要的字符）
// 每个字符用5字节表示，每个字节代表一行（从顶部到底部）
// 位模式：1=像素开启，0=像素关闭

// 字体数据：S, D, space, c, a, r, d, n, o, t, e, i
const uint8_t font_5x7[][5] = {
  {0x0E, 0x11, 0x11, 0x11, 0x0E}, // S (0)
  {0x1F, 0x11, 0x11, 0x11, 0x0E}, // D (1)
  {0x00, 0x00, 0x00, 0x00, 0x00}, // space (2)
  {0x0E, 0x11, 0x10, 0x11, 0x0E}, // c (3)
  {0x0E, 0x11, 0x11, 0x11, 0x0E}, // a (4)
  {0x1F, 0x08, 0x08, 0x08, 0x08}, // r (5)
  {0x1F, 0x11, 0x11, 0x11, 0x0E}, // d (6)
  {0x11, 0x11, 0x11, 0x11, 0x0E}, // n (7)
  {0x0E, 0x11, 0x11, 0x11, 0x0E}, // o (8)
  {0x10, 0x10, 0x1F, 0x10, 0x10}, // t (9)
  {0x0E, 0x11, 0x11, 0x11, 0x0E}, // e (10)
  {0x00, 0x00, 0x1F, 0x00, 0x00}, // i (11)
};

// 字符映射：S=0, D=1, space=2, c=3, a=4, r=5, d=6, n=7, o=8, t=9, e=10, i=11
int getCharIndex(char c) {
  switch(c) {
    case 'S': case 's': return 0;
    case 'D': return 1;
    case ' ': return 2;
    case 'c': return 3;
    case 'a': return 4;
    case 'r': return 5;
    case 'd': return 6;
    case 'n': return 7;
    case 'o': return 8;
    case 't': return 9;
    case 'e': return 10;
    case 'i': return 11;
    default: return 2; // 默认返回空格
  }
}

// 在指定位置绘制一个字符
void drawChar(int x, int y, char c, uint16_t color) {
  int charIndex = getCharIndex(c);
  const uint8_t* charData = font_5x7[charIndex];
  
  for (int row = 0; row < 7; row++) {
    uint8_t rowData = (row < 5) ? charData[row] : 0;
    for (int col = 0; col < 5; col++) {
      if (rowData & (0x10 >> col)) {
        int px = x + col;
        int py = y + row;
        if (px >= 0 && px < LCD_WIDTH && py >= 0 && py < LCD_HEIGHT) {
          uint16_t pixel[1] = {color};
          LCD_addWindow(px, py, px, py, pixel);
        }
      }
    }
  }
}

// 在指定位置绘制字符串
void drawString(int x, int y, const char* str, uint16_t color) {
  int pos = 0;
  while (*str) {
    drawChar(x + pos * 6, y, *str, color); // 字符间距6像素
    str++;
    pos++;
  }
}

void LCD_ShowError(const char* message) {
  // 填充屏幕为黑色
  LCD_FillScreen(RGB565(0, 0, 0));
  
  // 在屏幕中央绘制文字（分两行显示）
  uint16_t text_color = RGB565(255, 255, 255); // 白色文字
  int text_x = 20;
  int text_y1 = 100;  // 第一行
  int text_y2 = 120;  // 第二行
  
  // 计算字符串长度，如果太长则分两行显示
  int len = strlen(message);
  if (len > 20) {
    // 分两行显示
    char line1[21] = {0};
    char line2[21] = {0};
    int mid = len / 2;
    // 找到中间的空格位置
    for (int i = mid; i < len && i < mid + 10; i++) {
      if (message[i] == ' ') {
        mid = i;
        break;
      }
    }
    strncpy(line1, message, mid);
    strncpy(line2, message + mid + 1, len - mid);
    drawString(text_x, text_y1, line1, text_color);
    drawString(text_x, text_y2, line2, text_color);
  } else {
    // 单行显示
    drawString(text_x, text_y1, message, text_color);
  }
  
  // 串口输出
  Serial.printf("Error: %s\n", message);
}

