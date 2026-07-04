/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_UI_H
#define GUI_UI_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_GIF_Demo1;
	lv_obj_t *screen_line_1;
	lv_obj_t *screen_line_3;
	lv_obj_t *screen_City_label;
	lv_obj_t *screen_line_2;
	lv_obj_t *screen_Text_label;
	lv_obj_t *screen_digital_clock;
	lv_obj_t *screen_line_4;
	lv_obj_t *screen_line_5;
	lv_obj_t *screen_label_Week;
	lv_obj_t *screen_label_Date;
	lv_obj_t *screen_line_6;
	lv_obj_t *screen_line_7;
	lv_obj_t *screen_line_8;
	lv_obj_t *screen_RH_img;
	lv_obj_t *screen_T_amb_img;
	lv_obj_t *screen_label_T;
	lv_obj_t *screen_WaveShare_img;
	lv_obj_t *screen_label_RH;
	lv_obj_t *screen_label_Wheel;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui ui_ctx;


void setup_scr_screen(lv_ui *ui);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(screen_GIF_Demo1A1);
LV_IMG_DECLARE(screen_GIF_Demo1A9);
LV_IMG_DECLARE(screen_GIF_Demo1A10);
LV_IMG_DECLARE(screen_GIF_Demo1A11);
LV_IMG_DECLARE(screen_GIF_Demo1A12);
LV_IMG_DECLARE(screen_GIF_Demo1A13);
LV_IMG_DECLARE(screen_GIF_Demo1A14);
LV_IMG_DECLARE(screen_GIF_Demo1A15);
LV_IMG_DECLARE(screen_GIF_Demo1A16);
LV_IMG_DECLARE(screen_GIF_Demo1A17);
LV_IMG_DECLARE(screen_GIF_Demo1A18);
LV_IMG_DECLARE(screen_GIF_Demo1A2);
LV_IMG_DECLARE(screen_GIF_Demo1A19);
LV_IMG_DECLARE(screen_GIF_Demo1A20);
LV_IMG_DECLARE(screen_GIF_Demo1A3);
LV_IMG_DECLARE(screen_GIF_Demo1A4);
LV_IMG_DECLARE(screen_GIF_Demo1A5);
LV_IMG_DECLARE(screen_GIF_Demo1A6);
LV_IMG_DECLARE(screen_GIF_Demo1A7);
LV_IMG_DECLARE(screen_GIF_Demo1A8);
LV_IMG_DECLARE(_shidu_alpha_19x19);
LV_IMG_DECLARE(_wendu_alpha_19x19);
LV_IMG_DECLARE(_WaveShare_alpha_25x25);

LV_FONT_DECLARE(lv_font_YZGJHeiFont_18)
LV_FONT_DECLARE(lv_font_YZGJHeiFont_40)


#ifdef __cplusplus
}
#endif
#endif

