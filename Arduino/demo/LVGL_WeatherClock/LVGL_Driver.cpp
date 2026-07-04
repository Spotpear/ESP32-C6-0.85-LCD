/*****************************************************************************
  | File        :   LVGL_Driver.c
  
  | help        : 
    The provided LVGL library file must be installed first
******************************************************************************/
#include "LVGL_Driver.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t* buf1 = NULL;
static lv_color_t* buf2 = NULL;
    


/* Serial debugging */
void Lvgl_print(const char * buf)
{
    // Serial.printf(buf);
    // Serial.flush();
}

/*  Display flushing 
    Displays LVGL content on the LCD
    This function implements associating LVGL data to the LCD screen
*/
void Lvgl_Display_LCD( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  uint32_t size = w * h;
  
  // 转换LVGL颜色格式
  static uint16_t *buf = NULL;
  static uint32_t buf_size = 0;
  
  if (size > buf_size) {
    if (buf) heap_caps_free(buf);
    buf = (uint16_t*)heap_caps_malloc(size * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    if (!buf) buf = (uint16_t*)heap_caps_malloc(size * sizeof(uint16_t), MALLOC_CAP_8BIT);
    if (buf) buf_size = size;
  }
  
  if (buf && buf_size >= size) {
    // 复制颜色数据并交换字节序（RGB565格式需要大端字节序）
    for (uint32_t i = 0; i < size; i++) {
      uint16_t color = color_p[i].full;
      // 交换字节序：LVGL是小端，LCD需要大端
      buf[i] = color;
    }
    LCD_addWindow(area->x1, area->y1, area->x2, area->y2, buf);
  } else {
    // 如果内存分配失败，需要转换后写入
    // 不能直接使用原数据，因为字节序可能不对
    static uint16_t temp_buf[100];  // 临时小缓冲区
    uint32_t chunk_size = (size < 100) ? size : 100;
    for (uint32_t i = 0; i < chunk_size; i++) {
      uint16_t color = color_p[i].full;
      temp_buf[i] = (color << 8) | (color >> 8);
    }
    LCD_addWindow(area->x1, area->y1, area->x2, area->y2, temp_buf);
  }
  
  lv_disp_flush_ready( disp_drv );
}
/*Read the touchpad*/
void Lvgl_Touchpad_Read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data )
{
  // NULL
}
void example_increase_lvgl_tick(void *arg)
{
    /* In LVGL 8.3+, tick is handled via LV_TICK_CUSTOM in lv_conf.h */
    /* If LV_TICK_CUSTOM is enabled, this function is not needed */
    /* Otherwise, LVGL handles ticks automatically through timer_handler */
    /* This function can be empty or removed if using LV_TICK_CUSTOM */
}
void Lvgl_Init(void)
{
  lv_init();

  buf1 = (lv_color_t*)heap_caps_malloc(LVGL_BUF_LEN * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  if (!buf1) buf1 = (lv_color_t*)heap_caps_malloc(LVGL_BUF_LEN * sizeof(lv_color_t), MALLOC_CAP_8BIT);
  buf2 = (lv_color_t*)heap_caps_malloc(LVGL_BUF_LEN * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  if (!buf2) buf2 = (lv_color_t*)heap_caps_malloc(LVGL_BUF_LEN * sizeof(lv_color_t), MALLOC_CAP_8BIT);

  lv_disp_draw_buf_init( &draw_buf, buf1, buf2, LVGL_BUF_LEN);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = LVGL_WIDTH;
  disp_drv.ver_res = LVGL_HEIGHT;
  disp_drv.flush_cb = Lvgl_Display_LCD;
  disp_drv.full_refresh = 0;                    /**< 0: Partial refresh for better performance*/
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = Lvgl_Touchpad_Read;
  lv_indev_drv_register( &indev_drv );

  // 设置屏幕背景为黑色
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0000), 0);
  lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
  
  // 强制刷新整个屏幕为黑色
  lv_refr_now(NULL);

  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

}
void Timer_Loop(void)
{
  /* In LVGL 8.3+, tick is handled automatically via LV_TICK_CUSTOM */
  /* Just call lv_timer_handler() which handles everything */
  /* 必须频繁调用lv_timer_handler()以确保动画和定时器正常工作 */
  lv_timer_handler(); /* let the GUI do its work */
  // delay( 5 );  // 移除延迟，确保频繁调用
}