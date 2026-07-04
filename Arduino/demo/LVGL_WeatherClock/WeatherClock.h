#pragma once

#include <lvgl.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include "Display_ST7789.h"
#include <stddef.h>
#include "generated/gui_ui.h"
#include "generated/widgets_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "generated/ui_fonts/lv_customer_font_Acme_Regular_32.c"


// WiFi配置
extern const char* wifi_ssid;
extern const char* wifi_password;

// 城市代码
extern String cityCode;
extern String follower;
extern char* api_key;
extern const int cityAutoDetect;
extern float timeZone;

// 颜色定义
#define BG_COLOR 0x000000
#define TIME_HM_COLOR 0xFFFFFF
#define TIME_S_COLOR 0xFFFFFF
#define WEEK_COLOR 0xFFFFFF
#define MONTH_COLOR 0xFFFFFF
#define TH_COLOR 0xFFFFFF
#define TIP_COLOR 0xFFFFFF
#define CITY_COLOR 0xFFFFFF
#define AIR_COLOR 0xFFFFFF

// 函数声明
void WeatherClock_Init(void);
void WeatherClock_Update(void);
void WeatherClock_Loop(void);
void WeatherClock_ShowGifOnly(void);

// 天气图标相关
void updateWeatherIcon(int weatherCode);          // 纯LVGL版本不再绘制JPG
void updateTempHumidityIcon(bool showTemp);       // 纯LVGL版本不再绘制JPG


// WiFi和网络相关
void get_wifi(void);
void getCityCode(bool autoGetWeather = true);
void getCityWeater(void);
void getCityTime(void);
String httpGetRequest(String url, String userAgent, String referer);

// 时间相关
time_t getNtpTime(void);
void sendNTPpacket(IPAddress &address);
void digitalClockDisplay(void);
String week(void);
String monthDay(void);
String hourMinute(void);
String num2str(int digits);

// 天气显示相关
void weaterData(String *cityDZ, String *dataSK, String *dataFC);
void weatherWarning(void);
void scrollBanner(void);
void fanspush(void);

// 背光控制
void setBacklight(bool on);
void backlightControl(void);

// FreeRTOS 任务相关
void weatherTask(void *parameter);
void requestWeatherUpdate(void);


