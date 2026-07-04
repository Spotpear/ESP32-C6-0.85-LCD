/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_ui.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



int screen_digital_clock_min_value = 25;
int screen_digital_clock_hour_value = 11;
int screen_digital_clock_sec_value = 50;
void setup_scr_screen(lv_ui *ui)
{
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 240, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_GIF_Demo1
    ui->screen_GIF_Demo1 = lv_animimg_create(ui->screen);
    lv_animimg_set_src(ui->screen_GIF_Demo1, (const void **) screen_GIF_Demo1_imgs, 20, false);
    lv_animimg_set_duration(ui->screen_GIF_Demo1, 40*20);
    lv_animimg_set_repeat_count(ui->screen_GIF_Demo1, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(ui->screen_GIF_Demo1);
    lv_obj_set_pos(ui->screen_GIF_Demo1, 83, 87);
    lv_obj_set_size(ui->screen_GIF_Demo1, 75, 72);

    //Write codes screen_line_1
    ui->screen_line_1 = lv_line_create(ui->screen);
    static lv_point_t screen_line_1[] = {{0, 0},{240, 0},};
    lv_line_set_points(ui->screen_line_1, screen_line_1, 2);
    lv_obj_set_pos(ui->screen_line_1, 0, 35);
    lv_obj_set_size(ui->screen_line_1, 240, 1);

    //Write style for screen_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_line_3
    ui->screen_line_3 = lv_line_create(ui->screen);
    static lv_point_t screen_line_3[] = {{0, 0},{0, 240},};
    lv_line_set_points(ui->screen_line_3, screen_line_3, 2);
    lv_obj_set_pos(ui->screen_line_3, 100, 2);
    lv_obj_set_size(ui->screen_line_3, 1, 32);

    //Write style for screen_line_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_line_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_City_label
    ui->screen_City_label = lv_label_create(ui->screen);
    lv_label_set_text(ui->screen_City_label, "深圳");
    lv_label_set_long_mode(ui->screen_City_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_City_label, 4, 5);
    lv_obj_set_size(ui->screen_City_label, 91, 25);

    //Write style for screen_City_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_City_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_City_label, &lv_font_YZGJHeiFont_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_City_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_City_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_City_label, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_City_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_line_2
    ui->screen_line_2 = lv_line_create(ui->screen);
    static lv_point_t screen_line_2[] = {{0, 0},{240, 0},};
    lv_line_set_points(ui->screen_line_2, screen_line_2, 2);
    lv_obj_set_pos(ui->screen_line_2, 0, 2);
    lv_obj_set_size(ui->screen_line_2, 240, 1);

    //Write style for screen_line_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_line_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_Text_label
    ui->screen_Text_label = lv_label_create(ui->screen);
    lv_label_set_text(ui->screen_Text_label, "我思故我在");
    lv_label_set_long_mode(ui->screen_Text_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_Text_label, 105, 6);
    lv_obj_set_size(ui->screen_Text_label, 129, 26);

    //Write style for screen_Text_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_Text_label, lv_color_hex(0xdd3636), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_Text_label, &lv_font_YZGJHeiFont_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_Text_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_Text_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_Text_label, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_Text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_digital_clock
    static bool screen_digital_clock_timer_enabled = false;
    ui->screen_digital_clock = lv_dclock_create(ui->screen, "11:25:50");
    if (!screen_digital_clock_timer_enabled) {
        lv_timer_create(screen_digital_clock_timer, 1000, NULL);
        screen_digital_clock_timer_enabled = true;
    }
    lv_obj_set_pos(ui->screen_digital_clock, 34, 40);
    lv_obj_set_size(ui->screen_digital_clock, 168, 51);

    //Write style for screen_digital_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_digital_clock, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_digital_clock, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_digital_clock, &lv_font_YZGJHeiFont_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_digital_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_digital_clock, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_digital_clock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_digital_clock, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_line_4
    ui->screen_line_4 = lv_line_create(ui->screen);
    static lv_point_t screen_line_4[] = {{0, 0},{240, 0},};
    lv_line_set_points(ui->screen_line_4, screen_line_4, 2);
    lv_obj_set_pos(ui->screen_line_4, 0, 171);
    lv_obj_set_size(ui->screen_line_4, 240, 1);

    //Write style for screen_line_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_4, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_line_5
    ui->screen_line_5 = lv_line_create(ui->screen);
    static lv_point_t screen_line_5[] = {{0, 0},{240, 0},};
    lv_line_set_points(ui->screen_line_5, screen_line_5, 2);
    lv_obj_set_pos(ui->screen_line_5, 0, 205);
    lv_obj_set_size(ui->screen_line_5, 240, 1);

    //Write style for screen_line_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_5, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_line_5, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_Week
    ui->screen_label_Week = lv_label_create(ui->screen);
    lv_label_set_text(ui->screen_label_Week, "周五");
    lv_label_set_long_mode(ui->screen_label_Week, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_label_Week, 5, 178);
    lv_obj_set_size(ui->screen_label_Week, 53, 17);

    //Write style for screen_label_Week, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_Week, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_Week, &lv_font_YZGJHeiFont_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_Week, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_Week, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_Week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_Date
    ui->screen_label_Date = lv_label_create(ui->screen);
    lv_label_set_text(ui->screen_label_Date, "12�?5�?);
    lv_label_set_long_mode(ui->screen_label_Date, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_label_Date, 59, 178);
    lv_obj_set_size(ui->screen_label_Date, 84, 19);

    //Write style for screen_label_Date, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_Date, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_Date, &lv_font_YZGJHeiFont_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_Date, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_Date, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_Date, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_line_6
    ui->screen_line_6 = lv_line_create(ui->screen);
    static lv_point_t screen_line_6[] = {{0, 0},{240, 0},};
    lv_line_set_points(ui->screen_line_6, screen_line_6, 2);
    lv_obj_set_pos(ui->screen_line_6, 0, 238);
    lv_obj_set_size(ui->screen_line_6, 240, 1);

    //Write style for screen_line_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_6, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_line_6, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_line_7
    ui->screen_line_7 = lv_line_create(ui->screen);
    static lv_point_t screen_line_7[] = {{0, 0},{0, 240},};
    lv_line_set_points(ui->screen_line_7, screen_line_7, 2);
    lv_obj_set_pos(ui->screen_line_7, 58, 171);
    lv_obj_set_size(ui->screen_line_7, 1, 32);

    //Write style for screen_line_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_7, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_7, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_line_7, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_line_8
    ui->screen_line_8 = lv_line_create(ui->screen);
    static lv_point_t screen_line_8[] = {{0, 0},{0, 240},};
    lv_line_set_points(ui->screen_line_8, screen_line_8, 2);
    lv_obj_set_pos(ui->screen_line_8, 146, 171);
    lv_obj_set_size(ui->screen_line_8, 1, 32);

    //Write style for screen_line_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->screen_line_8, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->screen_line_8, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->screen_line_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->screen_line_8, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_RH_img
    ui->screen_RH_img = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_RH_img, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_RH_img, &_shidu_alpha_19x19);
    lv_img_set_pivot(ui->screen_RH_img, 50,50);
    lv_img_set_angle(ui->screen_RH_img, 0);
    lv_obj_set_pos(ui->screen_RH_img, 5, 146);
    lv_obj_set_size(ui->screen_RH_img, 19, 19);

    //Write style for screen_RH_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_RH_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_RH_img, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui->screen_RH_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_RH_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_T_amb_img
    ui->screen_T_amb_img = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_T_amb_img, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_T_amb_img, &_wendu_alpha_19x19);
    lv_img_set_pivot(ui->screen_T_amb_img, 50,50);
    lv_img_set_angle(ui->screen_T_amb_img, 0);
    lv_obj_set_pos(ui->screen_T_amb_img, 150, 178);
    lv_obj_set_size(ui->screen_T_amb_img, 19, 19);

    //Write style for screen_T_amb_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_T_amb_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_T_amb_img, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui->screen_T_amb_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_T_amb_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_T
    ui->screen_label_T = lv_label_create(ui->screen);
    lv_label_set_text(ui->screen_label_T, "333�?);
    lv_label_set_long_mode(ui->screen_label_T, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_label_T, 169, 178);
    lv_obj_set_size(ui->screen_label_T, 67, 18);

    //Write style for screen_label_T, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_T, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_T, &lv_font_YZGJHeiFont_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_T, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_T, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_T, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_WaveShare_img
    ui->screen_WaveShare_img = lv_img_create(ui->screen);
    lv_obj_add_flag(ui->screen_WaveShare_img, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->screen_WaveShare_img, &_WaveShare_alpha_25x25);
    lv_img_set_pivot(ui->screen_WaveShare_img, 50,50);
    lv_img_set_angle(ui->screen_WaveShare_img, 0);
    lv_obj_set_pos(ui->screen_WaveShare_img, 0, 209);
    lv_obj_set_size(ui->screen_WaveShare_img, 25, 25);

    //Write style for screen_WaveShare_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_WaveShare_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->screen_WaveShare_img, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui->screen_WaveShare_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->screen_WaveShare_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_RH
    ui->screen_label_RH = lv_label_create(ui->screen);
    lv_label_set_text(ui->screen_label_RH, "100%");
    lv_label_set_long_mode(ui->screen_label_RH, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_label_RH, 24, 146);
    lv_obj_set_size(ui->screen_label_RH, 51, 17);

    //Write style for screen_label_RH, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_RH, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_RH, &lv_font_YZGJHeiFont_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_RH, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_RH, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_RH, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_Wheel
    ui->screen_label_Wheel = lv_label_create(ui->screen);
    lv_label_set_text(ui->screen_label_Wheel, "我思故我在");
    lv_label_set_long_mode(ui->screen_label_Wheel, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->screen_label_Wheel, 31, 212);
    lv_obj_set_size(ui->screen_label_Wheel, 204, 20);

    //Write style for screen_label_Wheel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_Wheel, lv_color_hex(0xdd3636), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_Wheel, &lv_font_YZGJHeiFont_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_Wheel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_Wheel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_label_Wheel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen);

}

