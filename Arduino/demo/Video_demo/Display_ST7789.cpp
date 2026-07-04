#include "Display_ST7789.h"
   
#define SPI_WRITE(_dat)         SPI.transfer(_dat)
#define SPI_WRITE_Word(_dat)    SPI.transfer16(_dat)
void SPI_Init()
{
  SPI.begin(EXAMPLE_PIN_NUM_SCLK,EXAMPLE_PIN_NUM_MISO,EXAMPLE_PIN_NUM_MOSI); 
}

void LCD_WriteCommand(uint8_t Cmd)  
{ 
  SPI.beginTransaction(SPISettings(SPIFreq, MSBFIRST, SPI_MODE0));
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, LOW);  
  digitalWrite(EXAMPLE_PIN_NUM_LCD_DC, LOW); 
  SPI_WRITE(Cmd);
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, HIGH);  
  SPI.endTransaction();
}
void LCD_WriteData(uint8_t Data) 
{ 
  SPI.beginTransaction(SPISettings(SPIFreq, MSBFIRST, SPI_MODE0));
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, LOW);  
  digitalWrite(EXAMPLE_PIN_NUM_LCD_DC, HIGH);  
  SPI_WRITE(Data);  
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, HIGH);  
  SPI.endTransaction();
}    
void LCD_WriteData_Word(uint16_t Data)
{
  SPI.beginTransaction(SPISettings(SPIFreq, MSBFIRST, SPI_MODE0));
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, LOW);  
  digitalWrite(EXAMPLE_PIN_NUM_LCD_DC, HIGH); 
  SPI_WRITE_Word(Data);
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, HIGH);  
  SPI.endTransaction();
}   
void LCD_WriteData_nbyte(uint8_t* SetData,uint8_t* ReadData,uint32_t Size) 
{ 
  SPI.beginTransaction(SPISettings(SPIFreq, MSBFIRST, SPI_MODE0));
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, LOW);  
  digitalWrite(EXAMPLE_PIN_NUM_LCD_DC, HIGH);  
  // 逐字节传输，确保字节序正确
  // 由于数据已经是正确的字节序（在LVGL_Driver中已交换），直接传输
  for (uint32_t i = 0; i < Size; i++) {
    SPI_WRITE(SetData[i]);
  }
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, HIGH);  
  SPI.endTransaction();
} 

void LCD_Reset(void)
{
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, LOW);       
  delay(50);
  digitalWrite(EXAMPLE_PIN_NUM_LCD_RST, LOW); 
  delay(50);
  digitalWrite(EXAMPLE_PIN_NUM_LCD_RST, HIGH); 
  delay(50);
}
void LCD_Init(void)
{
  pinMode(EXAMPLE_PIN_NUM_LCD_CS, OUTPUT);
  pinMode(EXAMPLE_PIN_NUM_LCD_DC, OUTPUT);
  pinMode(EXAMPLE_PIN_NUM_LCD_RST, OUTPUT); 
  Backlight_Init();
  SPI_Init();

  LCD_Reset();
  //************* Start Initial Sequence **********// 
  LCD_WriteCommand(0x11);
  delay(120);
  LCD_WriteCommand(0x36);
  if (HORIZONTAL)
      LCD_WriteData(0x08);
  else
      LCD_WriteData(0xB0);

  LCD_WriteCommand(0x3A);
  LCD_WriteData(0x05);

  LCD_WriteCommand(0xB0);
  LCD_WriteData(0x00);
  LCD_WriteData(0xE8);
  
  LCD_WriteCommand(0xB2);
  LCD_WriteData(0x0C);
  LCD_WriteData(0x0C);
  LCD_WriteData(0x00);
  LCD_WriteData(0x33);
  LCD_WriteData(0x33);

  LCD_WriteCommand(0xB7);
  LCD_WriteData(0x35);

  LCD_WriteCommand(0xBB);
  LCD_WriteData(0x35);

  LCD_WriteCommand(0xC0);
  LCD_WriteData(0x2C);

  LCD_WriteCommand(0xC2);
  LCD_WriteData(0x01);

  LCD_WriteCommand(0xC3);
  LCD_WriteData(0x13);

  LCD_WriteCommand(0xC4);
  LCD_WriteData(0x20);

  LCD_WriteCommand(0xC6);
  LCD_WriteData(0x0F);

  LCD_WriteCommand(0xD0);
  LCD_WriteData(0xA4);
  LCD_WriteData(0xA1);

  LCD_WriteCommand(0xD6);
  LCD_WriteData(0xA1);

  LCD_WriteCommand(0xE0);
  LCD_WriteData(0xF0);
  LCD_WriteData(0x00);
  LCD_WriteData(0x04);
  LCD_WriteData(0x04);
  LCD_WriteData(0x04);
  LCD_WriteData(0x05);
  LCD_WriteData(0x29);
  LCD_WriteData(0x33);
  LCD_WriteData(0x3E);
  LCD_WriteData(0x38);
  LCD_WriteData(0x12);
  LCD_WriteData(0x12);
  LCD_WriteData(0x28);
  LCD_WriteData(0x30);

  LCD_WriteCommand(0xE1);
  LCD_WriteData(0xF0);
  LCD_WriteData(0x07);
  LCD_WriteData(0x0A);
  LCD_WriteData(0x0D);
  LCD_WriteData(0x0B);
  LCD_WriteData(0x07);
  LCD_WriteData(0x28);
  LCD_WriteData(0x33);
  LCD_WriteData(0x3E);
  LCD_WriteData(0x36);
  LCD_WriteData(0x14);
  LCD_WriteData(0x14);
  LCD_WriteData(0x29);
  LCD_WriteData(0x32);

  LCD_WriteCommand(0x21);

  LCD_WriteCommand(0x11);
  delay(120);
  LCD_WriteCommand(0x29);
  
  // 初始化后清屏为黑色
  LCD_FillScreen(0x0000);
}
/******************************************************************************
function: Set the cursor position
parameter :
    Xstart:   Start uint16_t x coordinate
    Ystart:   Start uint16_t y coordinate
    Xend  :   End uint16_t coordinates
    Yend  :   End uint16_t coordinatesen
******************************************************************************/
void LCD_SetCursor(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t  Yend)
{ 
  
  // 确保坐标在有效范围内
  if (Xstart >= LCD_WIDTH) Xstart = LCD_WIDTH - 1;
  if (Ystart >= LCD_HEIGHT) Ystart = LCD_HEIGHT - 1;
  if (Xend >= LCD_WIDTH) Xend = LCD_WIDTH - 1;
  if (Yend >= LCD_HEIGHT) Yend = LCD_HEIGHT - 1;
  
  if (HORIZONTAL) {
    // set the X coordinates
    LCD_WriteCommand(0x2A);
    LCD_WriteData((Xstart + Offset_X) >> 8);
    LCD_WriteData((Xstart + Offset_X) & 0xFF);
    LCD_WriteData((Xend + Offset_X) >> 8);
    LCD_WriteData((Xend + Offset_X) & 0xFF);
    
    // set the Y coordinates
    LCD_WriteCommand(0x2B);
    LCD_WriteData((Ystart + Offset_Y) >> 8);
    LCD_WriteData((Ystart + Offset_Y) & 0xFF);
    LCD_WriteData((Yend + Offset_Y) >> 8);
    LCD_WriteData((Yend + Offset_Y) & 0xFF);
  }
  else {
    // VERTICAL模式：X和Y交换
    // set the X coordinates (实际是Y)
    LCD_WriteCommand(0x2A);
    LCD_WriteData((Ystart + Offset_Y) >> 8);
    LCD_WriteData((Ystart + Offset_Y) & 0xFF);
    LCD_WriteData((Yend + Offset_Y) >> 8);
    LCD_WriteData((Yend + Offset_Y) & 0xFF);
    // set the Y coordinates (实际是X)
    LCD_WriteCommand(0x2B);
    LCD_WriteData((Xstart + Offset_X) >> 8);
    LCD_WriteData((Xstart + Offset_X) & 0xFF);
    LCD_WriteData((Xend + Offset_X) >> 8);
    LCD_WriteData((Xend + Offset_X) & 0xFF);
  }
  LCD_WriteCommand(0x2C);
}
/******************************************************************************
function: Refresh the image in an area
parameter :
    Xstart:   Start uint16_t x coordinate
    Ystart:   Start uint16_t y coordinate
    Xend  :   End uint16_t coordinates
    Yend  :   End uint16_t coordinates
    color :   Set the color
******************************************************************************/
void LCD_addWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,uint16_t* color)
{          
  uint16_t Show_Width = Xend - Xstart + 1;
  uint16_t Show_Height = Yend - Ystart + 1;
  uint32_t totalPixels = Show_Width * Show_Height;
  
  LCD_SetCursor(Xstart, Ystart, Xend, Yend);
  
  // 使用批量传输，但确保字节序正确
  // 数据已经在LVGL_Driver中交换过字节序，所以直接传输
  SPI.beginTransaction(SPISettings(SPIFreq, MSBFIRST, SPI_MODE0));
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, LOW);  
  digitalWrite(EXAMPLE_PIN_NUM_LCD_DC, HIGH);
  
  // 批量传输16位数据
  for (uint32_t i = 0; i < totalPixels; i++) {
    uint16_t pixel = color[i];
    // 使用transfer16确保正确的字节序
    SPI_WRITE_Word(pixel);
  }
  
  digitalWrite(EXAMPLE_PIN_NUM_LCD_CS, HIGH);  
  SPI.endTransaction();
}
// backlight
void Backlight_Init(void)
{
  ledcAttach(EXAMPLE_PIN_NUM_BK_LIGHT, Frequency, Resolution);   
  ledcWrite(EXAMPLE_PIN_NUM_BK_LIGHT, 100);                        
}

void Set_Backlight(uint8_t Light)                        //
{

  if(Light > 100 || Light < 0)
    printf("Set Backlight parameters in the range of 0 to 100 \r\n");
  else{
    uint32_t Backlight = Light*10;
    ledcWrite(EXAMPLE_PIN_NUM_BK_LIGHT, Backlight);
  }
}
// RGB转RGB565
uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
// 填充整个屏幕为指定颜色
void LCD_FillScreen(uint16_t color) {
  // 创建一行颜色数据
  static uint16_t color_line[LCD_WIDTH];
  for (int i = 0; i < LCD_WIDTH; i++) {
    color_line[i] = color;
  }
  
  // 逐行填充
  for (int y = 0; y < LCD_HEIGHT; y++) {
    LCD_addWindow(0, y, LCD_WIDTH - 1, y, color_line);
  }
}





