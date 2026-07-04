/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_ui.h"
#include "widgets_init.h"
#include <stdlib.h>
#include <string.h>


const lv_img_dsc_t * screen_GIF_Demo1_imgs[20] = {
    &screen_GIF_Demo1A1,
    &screen_GIF_Demo1A9,
    &screen_GIF_Demo1A10,
    &screen_GIF_Demo1A11,
    &screen_GIF_Demo1A12,
    &screen_GIF_Demo1A13,
    &screen_GIF_Demo1A14,
    &screen_GIF_Demo1A15,
    &screen_GIF_Demo1A16,
    &screen_GIF_Demo1A17,
    &screen_GIF_Demo1A18,
    &screen_GIF_Demo1A2,
    &screen_GIF_Demo1A19,
    &screen_GIF_Demo1A20,
    &screen_GIF_Demo1A3,
    &screen_GIF_Demo1A4,
    &screen_GIF_Demo1A5,
    &screen_GIF_Demo1A6,
    &screen_GIF_Demo1A7,
    &screen_GIF_Demo1A8,
};
/* stripped timer stub: original UI logic未使�?*/
void screen_digital_clock_timer(lv_timer_t *timer) { LV_UNUSED(timer); }


