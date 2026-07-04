#include "WeatherClock.h"
#include "LVGL_Driver.h"
#include <HTTPClient.h>

extern volatile bool videoModeActive;

// WiFi配置
const char* wifi_ssid = "spotpear";
const char* wifi_password = "spotpear123";

// OpenWeatherMap API配置
String cityCode = "Shenzhen";
String follower = "Hello";
char* api_key = "fbf5a0e942e6fea3ff18103b9fd46ed9";
const int cityAutoDetect = 1;

// NTP服务器配置
static const char ntpServerName[] = "ntp6.aliyun.com";
float timeZone = 8.0;
WiFiUDP Udp;
unsigned int localPort = 8000;
WiFiClient wificlient;

// 时间同步参数
byte setNTPSyncTime = 20;
time_t prevDisplay = 0;

// 天气更新参数
unsigned long weaterTime = 0;
unsigned long wdsdTime = 0;
byte wdsdValue = 0;
String wendu1 = "";
String wendu2 = "";
String shidu = "";
String yaqiang = "";
String tianqi = "";
String kjd = "";

// 温度变量
int temp_i1 = 0, temp_i2 = 0;

// 滚动文字
String scrollText[5];
int currentIndex = 0;
unsigned long prevTime = 0;

// LVGL对象
static lv_obj_t *time_hm_label = NULL;
static lv_obj_t *time_s_label = NULL;
static lv_obj_t *week_label = NULL;
static lv_obj_t *monthday_label = NULL;
static lv_obj_t *city_label = NULL;
static lv_obj_t *temp_label = NULL;
static lv_obj_t *humidity_label = NULL;
static lv_obj_t *air_label = NULL;
static lv_obj_t *battery_label = NULL;
static lv_obj_t *tip_label = NULL;
static lv_obj_t *wind_label = NULL;
static lv_obj_t *follower_label = NULL;
static lv_obj_t *now_label = NULL;
static lv_obj_t *gif_img = NULL;
static lv_obj_t *min_temp_label = NULL;
static lv_obj_t *weather_icon_img = NULL;
static lv_obj_t *temp_icon_img = NULL;
static lv_obj_t *humidity_icon_img = NULL;
static lv_obj_t *line_top = NULL;
static lv_obj_t *line_mid = NULL;
static lv_obj_t *line_bottom = NULL;
static lv_obj_t *line_vertical = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *hint_label = NULL;

// 样式
static lv_style_t style_time_hm;
static lv_style_t style_time_s;
static lv_style_t style_week;
static lv_style_t style_monthday;
static lv_style_t style_city;
static lv_style_t style_temp;
static lv_style_t style_humidity;
static lv_style_t style_air;
static lv_style_t style_battery;
static lv_style_t style_tip;
static lv_style_t style_wind;
static lv_style_t style_follower;
static lv_style_t style_now;

// 背光控制
bool AutoBright = false;
unsigned long backlightTime = 0;
  
// 电池电量（模拟，实际需要读取电池ADC）
int batteryLevel = 69;

// FreeRTOS 任务相关
static TaskHandle_t weatherTaskHandle = NULL;
static SemaphoreHandle_t weatherRequestSem = NULL;
static bool weatherUpdateRequested = false;
static bool cityCodeInitialized = false;
static bool initialWeatherReady = false;
static bool clockScreenReady = false;
static bool wifiTimedOut = false;
static const lv_font_t *font_status = &lv_font_YZGJHeiFont_18;

// 状态页展示
static void showStatusScreen(const char *hintText, const char *statusText) {
  lv_obj_clean(lv_scr_act());

  time_hm_label = NULL;
  time_s_label = NULL;
  week_label = NULL;
  monthday_label = NULL;
  city_label = NULL;
  temp_label = NULL;
  humidity_label = NULL;
  air_label = NULL;
  battery_label = NULL;
  tip_label = NULL;
  wind_label = NULL;
  follower_label = NULL;
  now_label = NULL;
  gif_img = NULL;
  min_temp_label = NULL;
  weather_icon_img = NULL;
  temp_icon_img = NULL;
  humidity_icon_img = NULL;
  line_top = NULL;
  line_mid = NULL;
  line_bottom = NULL;
  line_vertical = NULL;
  status_label = NULL;
  hint_label = NULL;

  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(BG_COLOR), 0);
  lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
  lv_obj_set_size(lv_scr_act(), LCD_WIDTH, LCD_HEIGHT);
  lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

  hint_label = lv_label_create(lv_scr_act());
  lv_label_set_text(hint_label, hintText);
  lv_obj_set_style_text_color(hint_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(hint_label, font_status, 0);
  lv_label_set_long_mode(hint_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(hint_label, LCD_WIDTH - 20);
  lv_obj_set_pos(hint_label, 5, 5);

  status_label = lv_label_create(lv_scr_act());
  lv_label_set_text(status_label, statusText);
  lv_obj_set_style_text_color(status_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(status_label, font_status, 0);
  lv_label_set_long_mode(status_label, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(status_label, LCD_WIDTH - 20);
  lv_obj_set_pos(status_label, 5, 55);
}

// HTTP请求函数（使用HTTPClient替代手动HTTP请求）
String httpGetRequest(String url, String userAgent, String referer) {
  String response = "";
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("HTTP request failed: WiFi not connected");
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
    return "";
  }
  
  HTTPClient httpClient;
  httpClient.begin(wificlient, url);
  
  if (userAgent.length() > 0) {
    httpClient.setUserAgent(userAgent);
  }
  if (referer.length() > 0) {
    httpClient.setUserAgent(referer);
  }
  
  Serial.print("Sending GET request: ");
  Serial.println(url);
  
  int httpCode = httpClient.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    response = httpClient.getString();
    Serial.print("HTTP response OK, length: ");
    Serial.println(response.length());
  } else {
    Serial.print("HTTP request failed, error code: ");
    Serial.println(httpCode);
  }
  
  httpClient.end();
  
  return response;
}

// 获取城市代码（使用IP-API）
void getCityCode(bool autoGetWeather) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to get city: WiFi not connected");
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());
    return;
  }
  
  String URL = "http://ip-api.com/json/?fields=city,lat,lon";
  Serial.print("Send GET request to URL: ");
  Serial.println(URL);
  
  String str = httpGetRequest(URL, "", "");
  
  Serial.print("Response length: ");
  Serial.println(str.length());
  
  if (str.length() > 0) {
    Serial.print("Response content (first 200 chars): ");
    Serial.println(str.substring(0, 200));
    
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, str);
    
    if (!error) {
      String city = doc["city"].as<String>();
      if (city.length() > 0 && cityAutoDetect == 1) {
        cityCode = city;
        Serial.print("Auto-detected city: ");
        Serial.println(cityCode);
        cityCodeInitialized = true;
        if (autoGetWeather) {
          getCityWeater();
        }
      } else {
        Serial.println("No city name found, using manual setting");
        cityCodeInitialized = true;
        if (autoGetWeather) {
          getCityWeater();
        }
      }
    } else {
      Serial.print("JSON parse failed: ");
      Serial.println(error.c_str());
      cityCodeInitialized = true;
      if (autoGetWeather) {
        getCityWeater();
      }
    }
  } else {
    Serial.println("City code request failed: connection error or empty response");
    Serial.println("Trying default city");
    cityCode = "Shenzhen";
    cityCodeInitialized = true;
    if (autoGetWeather) {
      getCityWeater();
    }
  }
}

// 获取城市和时区信息
void getCityTime() {
  getCityCode(false);
  
  String URL = "http://api.openweathermap.org/data/2.5/weather?q=" + cityCode + "&appid=" + String(api_key) + "&units=metric";
  
  Serial.println("Getting city timezone info");
  Serial.println(URL);
  
  String str = httpGetRequest(URL, "", "");
  
  if (str.length() > 0) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, str);
    
    if (!error) {
      long timezone_offset = doc["timezone"];
      timeZone = (float)(timezone_offset / 3600.0);
      Serial.print("Got timezone: ");
      Serial.println(timeZone);
      Serial.println("City timezone info retrieved successfully");
    } else {
      Serial.print("JSON parse failed: ");
      Serial.println(error.c_str());
      Serial.println("Failed to get city timezone");
    }
  } else {
    Serial.println("Failed to get city timezone");
  }
}

// 获取城市天气（使用OpenWeatherMap API）
void getCityWeater() {
  String URL = "http://api.openweathermap.org/data/2.5/weather?q=" + cityCode + "&appid=" + String(api_key) + "&units=metric";
  
  Serial.println("Getting weather data");
  Serial.println(URL);
  initialWeatherReady = false;
  
  String str = httpGetRequest(URL, "", "");
  
  if (str.length() > 0) {
    const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + JSON_OBJECT_SIZE(40) + 470;
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, str);
    
    if (!error) {
      float temp = doc["main"]["temp"];
      int humidity = doc["main"]["humidity"];
      int pressure = doc["main"]["pressure"];
      String description = doc["weather"][0]["description"].as<String>();
      String icon = doc["weather"][0]["icon"].as<String>();
      float temp_min = doc["main"]["temp_min"];
      float temp_max = doc["main"]["temp_max"];
      int visibility = doc["visibility"];
      
      float temp_f = 32 + temp * 1.8;
      float temp_min_f = 32 + temp_min * 1.8;
      float temp_max_f = 32 + temp_max * 1.8;
      
      temp_i1 = int(temp_f);
      temp_i2 = int(temp);
      
      wendu1 = String(temp_i1);
      wendu2 = String(temp_i2);
      shidu = String(humidity);
      yaqiang = String(pressure);
      tianqi = description;
      kjd = String(visibility / 1000);
      
      if (city_label) {
        lv_label_set_text(city_label, cityCode.c_str());
      }
      
      if (temp_label) {
        String tempStr = wendu2 + "℃";
        lv_label_set_text(temp_label, tempStr.c_str());
        lv_obj_invalidate(temp_label);
      }
      
      if (humidity_label) {
        String humidityStr = shidu + "%";
        lv_label_set_text(humidity_label, humidityStr.c_str());
        lv_obj_invalidate(humidity_label);
      }
      
      if (air_label) {
        String airStr = yaqiang + "hPa";
        lv_label_set_text(air_label, airStr.c_str());
      }
      
      if (wind_label) {
        lv_label_set_text(wind_label, tianqi.c_str());
      }
      
      scrollText[0] = "MIN T " + String(temp_min_f, 0) + "\u2109  " + String(temp_min, 0) + "\u2103";
      scrollText[1] = "MAX T " + String(temp_max_f, 0) + "\u2109  " + String(temp_max, 0) + "\u2103";
      scrollText[2] = "Weather " + tianqi;
      scrollText[3] = "Pressure " + yaqiang + "hPa";
      scrollText[4] = "Visibility " + kjd + "km";
      
      Serial.println("Weather data retrieved successfully");
      Serial.print("Temp: ");
      Serial.print(temp_i2);
      Serial.print("℃ / ");
      Serial.print(temp_i1);
      Serial.println("F");
      Serial.print("Humidity: ");
      Serial.println(shidu);
      Serial.print("Weather: ");
      Serial.println(tianqi);
      
      initialWeatherReady = true;
    } else {
      Serial.print("JSON Parsing failed: ");
      Serial.println(error.c_str());
      initialWeatherReady = false;
    }
  } else {
    Serial.println("Failed to request weather data");
    initialWeatherReady = false;
  }
}

// 解析天气数据（保留空函数以兼容，实际已不使用）
void weaterData(String *cityDZ, String *dataSK, String *dataFC) {
  Serial.println("weaterData function deprecated");
}

// 更新天气图标（根据天气代码，使用JPG数据）
void updateWeatherIcon(int weatherCode) {
  if (weather_icon_img == NULL) return;
 lv_img_set_zoom(weather_icon_img, 128);
  LV_UNUSED(weatherCode);
}

// 更新温湿度图标显示（使用JPG数据）
void updateTempHumidityIcon(bool showTemp) {
  
  LV_UNUSED(showTemp);
}

// 温湿度轮播显示
void weatherWarning() {
  if (millis() - wdsdTime > 5000) {
    wdsdValue = (wdsdValue + 1) % 3;
    
    if (temp_label && humidity_label) {
      switch (wdsdValue) {
        case 0:
        case 1:
          if (temp_label && wendu1.length() > 0) {
            String tempStr = wendu1 + "\u2109";
            lv_label_set_text(temp_label, tempStr.c_str());
            lv_obj_invalidate(temp_label);
          }
          break;
        case 2:
          if (temp_label && wendu2.length() > 0) {
            String tempStr = wendu2 + "\u2103";
            lv_label_set_text(temp_label, tempStr.c_str());
            lv_obj_invalidate(temp_label);
          }
          if (humidity_label && shidu.length() > 0) {
            String humidityStr = shidu + "%";
            lv_label_set_text(humidity_label, humidityStr.c_str());
            lv_obj_invalidate(humidity_label);
          }
          break;
      }
    }
    
    wdsdTime = millis();
  }
}

// 滚动文字（3.5秒切换）
void scrollBanner() {
  if (millis() - prevTime > 3500) {
    if (scrollText[currentIndex].length() > 0 && tip_label) {
      lv_label_set_text(tip_label, scrollText[currentIndex].c_str());
      lv_obj_invalidate(tip_label);
      
      lv_anim_t a;
      lv_anim_init(&a);
      lv_anim_set_var(&a, tip_label);
      lv_anim_set_values(&a, 116, 106);
      lv_anim_set_time(&a, 300);
      lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
      lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
      lv_anim_start(&a);
      
      if (currentIndex >= 4) {
        currentIndex = 0;
      } else {
        currentIndex += 1;
      }
    }
    prevTime = millis();
  }
}

// 粉丝数显示
void fanspush() {
  if (follower_label) {
    lv_label_set_text(follower_label, follower.c_str());
  }
}

// WiFi连接
void get_wifi() {
  wifiTimedOut = false;
  if (status_label) {
    lv_timer_handler();
  }
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    lv_timer_handler();
    
    if (millis() - startTime > 30000) {
      Serial.println("");
      Serial.println("WiFi connection timeout!");
      Serial.println("Continuing without WiFi...");
      wifiTimedOut = true;
      if (status_label) {
        lv_label_set_text(status_label, "Wi-Fi connection failed");
        lv_timer_handler();
      }
      return;
    }
    yield();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (status_label) {
    lv_label_set_text(status_label, "WiFi connection get city and weather");
    lv_timer_handler();
  }
}

// NTP时间获取
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

time_t getNtpTime() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("NTP sync failed: WiFi not connected");
    return 0;
  }
  
  getCityTime();
  
  IPAddress ntpServerIP;
  
  while (Udp.parsePacket() > 0) {
    Udp.flush();
  }
  
  if (WiFi.hostByName(ntpServerName, ntpServerIP) == 0) {
    Serial.println("NTP sync failed: cannot resolve NTP server");
    return 0;
  }
  
  Serial.print("Connecting to NTP server: ");
  Serial.print(ntpServerName);
  Serial.print(" (");
  Serial.print(ntpServerIP);
  Serial.println(")");
  
  sendNTPpacket(ntpServerIP);
  
  uint32_t beginWait = millis();
  while (millis() - beginWait < 3000) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("NTP sync success!");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      time_t timeValue = secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
      Serial.print("NTP time: ");
      Serial.println(timeValue);
      return timeValue;
    }
    delay(10);
    if (!videoModeActive) lv_timer_handler();
  }
  Serial.println("NTP sync failed: timeout");
  return 0;
}

void sendNTPpacket(IPAddress &address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

// 时间显示函数
String week() {
  int dayOfWeek = weekday() - 1;
  if (dayOfWeek < 0 || dayOfWeek >= 7) {
    dayOfWeek = 0;
  }
  
  String wk[7] = { "Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat." };
  String s = wk[dayOfWeek];
  
  return s;
}

String monthDay() {
  String s = String(month());
  s = s + "." + day() + "";
  return s;
}

String hourMinute() {
  String s = num2str(hour());
  s = s + ":" + num2str(minute());
  return s;
}

String num2str(int digits) {
  String s = "";
  if (digits < 10)
    s = s + "0";
  s = s + digits;
  return s;
}

void digitalClockDisplay() {
  time_t currentTime = now();
  
  if (currentTime > 0 && currentTime != prevDisplay) {
    prevDisplay = currentTime;

    if (time_hm_label) {
      String timeStr = num2str(hour()) + ":" + num2str(minute()) + ":" + num2str(second());
      lv_label_set_text(time_hm_label, timeStr.c_str());
      lv_obj_invalidate(time_hm_label);
    }

    if (week_label) {
      lv_label_set_text(week_label, week().c_str());
      lv_obj_invalidate(week_label);
    }
    
    if (monthday_label) {
      lv_label_set_text(monthday_label, monthDay().c_str());
      lv_obj_invalidate(monthday_label);
    }
  } else if (currentTime == 0) {
    static unsigned long lastSyncCheck = 0;
    if (millis() - lastSyncCheck > 5000) {
      lastSyncCheck = millis();
      Serial.println("Waiting for NTP time sync...");
      if (time_hm_label) {
        lv_label_set_text(time_hm_label, "Syncing...");
        lv_obj_invalidate(time_hm_label);
      }
    }
  }
}

// 背光控制
void setBacklight(bool on) {
  digitalWrite(EXAMPLE_PIN_NUM_BK_LIGHT, on ? LOW : HIGH);
  Serial.print("Backlight: ");
  Serial.println(on ? "ON" : "OFF");
}

void backlightControl() {
  if (AutoBright) {
    int currentHour = hour();
    if (currentHour >= 22 || currentHour < 6) {
      setBacklight(false);
    } else {
      setBacklight(true);
    }
  }
}

// 创建线框装饰
void createDecorativeLines(void) {
  line_top = lv_line_create(lv_scr_act());
  static lv_point_t line_top_points[] = {{0, 0}, {240, 0}};
  lv_line_set_points(line_top, line_top_points, 3);
  lv_obj_set_style_line_color(line_top, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_top, 2, 0);
  lv_obj_set_style_line_opa(line_top, LV_OPA_COVER, 0);

  static lv_obj_t *line_top2 = lv_line_create(lv_scr_act());
  static lv_point_t line_top2_points[] = {{0, 0}, {240, 0}};
  lv_line_set_points(line_top2, line_top2_points, 2);
  lv_obj_set_pos(line_top2, 0, 35);
  lv_obj_set_style_line_color(line_top2, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_top2, 2, 0);
  lv_obj_set_style_line_opa(line_top2, LV_OPA_COVER, 0);

  static lv_obj_t *line_vertical_top = lv_line_create(lv_scr_act());
  static lv_point_t line_vt_points[] = {{0, 0}, {0, 32}};
  lv_line_set_points(line_vertical_top, line_vt_points, 2);
  lv_obj_set_pos(line_vertical_top, 100, 2);
  lv_obj_set_size(line_vertical_top, 1, 32);
  lv_obj_set_style_line_color(line_vertical_top, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_vertical_top, 2, 0);
  lv_obj_set_style_line_opa(line_vertical_top, LV_OPA_COVER, 0);

  line_bottom = lv_line_create(lv_scr_act());
  static lv_point_t line_bottom_points[] = {{0, 0}, {240, 0}};
  lv_line_set_points(line_bottom, line_bottom_points, 2);
  lv_obj_set_pos(line_bottom, 0, 171);
  lv_obj_set_style_line_color(line_bottom, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_bottom, 2, 0);
  lv_obj_set_style_line_opa(line_bottom, LV_OPA_COVER, 0);

  static lv_obj_t *line_bottom2 = lv_line_create(lv_scr_act());
  static lv_point_t line_bottom2_points[] = {{0, 0}, {240, 0}};
  lv_line_set_points(line_bottom2, line_bottom2_points, 2);
  lv_obj_set_pos(line_bottom2, 0, 205);
  lv_obj_set_style_line_color(line_bottom2, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_bottom2, 2, 0);
  lv_obj_set_style_line_opa(line_bottom2, LV_OPA_COVER, 0);

  static lv_obj_t *line_bottom3 = lv_line_create(lv_scr_act());
  static lv_point_t line_bottom3_points[] = {{0, 0}, {240, 0}};
  lv_line_set_points(line_bottom3, line_bottom3_points, 2);
  lv_obj_set_pos(line_bottom3, 0, 238);
  lv_obj_set_style_line_color(line_bottom3, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_bottom3, 2, 0);
  lv_obj_set_style_line_opa(line_bottom3, LV_OPA_COVER, 0);

  static lv_obj_t *line_vertical1 = lv_line_create(lv_scr_act());
  static lv_point_t line_v1_points[] = {{0, 0}, {0, 34}};
  lv_line_set_points(line_vertical1, line_v1_points, 2);
  lv_obj_set_pos(line_vertical1, 58, 171);
  lv_obj_set_size(line_vertical1, 1, 34);
  lv_obj_set_style_line_color(line_vertical1, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_vertical1, 2, 0);
  lv_obj_set_style_line_opa(line_vertical1, LV_OPA_COVER, 0);

  static lv_obj_t *line_vertical2 = lv_line_create(lv_scr_act());
  static lv_point_t line_v2_points[] = {{0, 0}, {0, 34}};
  lv_line_set_points(line_vertical2, line_v2_points, 2);
  lv_obj_set_pos(line_vertical2, 146, 171);
  lv_obj_set_size(line_vertical2, 1, 34);
  lv_obj_set_style_line_color(line_vertical2, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_line_width(line_vertical2, 2, 0);
  lv_obj_set_style_line_opa(line_vertical2, LV_OPA_COVER, 0);
}

static void buildClockScreen(void) {
  lv_obj_clean(lv_scr_act());
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(BG_COLOR), 0);
  lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
  
  lv_obj_set_style_clip_corner(lv_scr_act(), true, 0);
  lv_obj_set_size(lv_scr_act(), LCD_WIDTH, LCD_HEIGHT);
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);
  lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

  const lv_font_t *font_large = LV_FONT_DEFAULT;
  const lv_font_t *font_mid = LV_FONT_DEFAULT;
  const lv_font_t *font_small = LV_FONT_DEFAULT;

  font_large = &lv_customer_font_Acme_Regular_32;
  font_mid = &lv_font_YZGJHeiFont_18;
  font_small = &lv_font_YZGJHeiFont_18;

  lv_style_init(&style_time_hm);
  lv_style_set_text_color(&style_time_hm, lv_color_hex(TIME_HM_COLOR));
  lv_style_set_text_font(&style_time_hm, font_large);

  lv_style_init(&style_week);
  lv_style_set_text_color(&style_week, lv_color_hex(WEEK_COLOR));
  lv_style_set_text_font(&style_week, font_mid);

  lv_style_init(&style_monthday);
  lv_style_set_text_color(&style_monthday, lv_color_hex(MONTH_COLOR));
  lv_style_set_text_font(&style_monthday, font_mid);

  lv_style_init(&style_city);
  lv_style_set_text_color(&style_city, lv_color_hex(CITY_COLOR));
  lv_style_set_text_font(&style_city, font_mid);
  lv_style_set_text_align(&style_city, LV_TEXT_ALIGN_CENTER);

  lv_style_init(&style_temp);
  lv_style_set_text_color(&style_temp, lv_color_hex(TH_COLOR));
  lv_style_set_text_font(&style_temp, font_mid);

  lv_style_init(&style_humidity);
  lv_style_set_text_color(&style_humidity, lv_color_hex(TH_COLOR));
  lv_style_set_text_font(&style_humidity, font_mid);
    lv_style_set_text_align(&style_humidity, LV_TEXT_ALIGN_CENTER);

  lv_style_init(&style_tip);
  lv_style_set_text_color(&style_tip, lv_color_hex(TIP_COLOR));
  lv_style_set_text_font(&style_tip, font_mid);
  lv_style_set_text_align(&style_tip, LV_TEXT_ALIGN_CENTER);

  lv_style_init(&style_follower);
  lv_style_set_text_color(&style_follower, lv_color_hex(0xFFFFFF));
  lv_style_set_text_font(&style_follower, font_mid);
  lv_style_set_text_align(&style_follower, LV_TEXT_ALIGN_CENTER);

  city_label = lv_label_create(lv_scr_act());
  lv_label_set_text(city_label, "Shenzhen");
  lv_obj_add_style(city_label, &style_city, 0);
  lv_obj_set_pos(city_label, 4, 2);
  lv_obj_set_width(city_label, 38);
  lv_label_set_long_mode(city_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_label_set_text(city_label, cityCode.c_str());

  time_hm_label = lv_label_create(lv_scr_act());
  lv_label_set_text(time_hm_label, "00:00:00");
  lv_obj_add_style(time_hm_label, &style_time_hm, 0);
  lv_obj_set_pos(time_hm_label, 0, 25);
  lv_obj_set_size(time_hm_label, 128, 36);

  tip_label = lv_label_create(lv_scr_act());
  lv_label_set_text(tip_label, "Real-time weather");
  lv_obj_add_style(tip_label, &style_tip, 0);
  lv_obj_set_pos(tip_label, 0, 106);
  lv_obj_set_size(tip_label, 128, 18);
  lv_obj_set_style_text_font(tip_label, font_small, 0);
  lv_label_set_long_mode(tip_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_style_clip_corner(tip_label, true, 0);
  lv_obj_clear_flag(tip_label, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(tip_label, LV_SCROLLBAR_MODE_OFF);
  lv_label_set_long_mode(tip_label, LV_LABEL_LONG_SCROLL_CIRCULAR);

  humidity_label = lv_label_create(lv_scr_act());
  lv_label_set_text(humidity_label, "--");
  lv_obj_add_style(humidity_label, &style_humidity, 0);
  lv_obj_set_pos(humidity_label, 50, 2);
  lv_obj_set_style_bg_color(humidity_label, lv_color_hex(BG_COLOR), 0);
  lv_obj_set_style_bg_opa(humidity_label, LV_OPA_COVER, 0);

  temp_label = lv_label_create(lv_scr_act());
  lv_label_set_text(temp_label, "--\u2103");
  lv_obj_add_style(temp_label, &style_temp, 0);
  lv_obj_set_pos(temp_label, 4, 85);
  lv_obj_set_style_bg_color(temp_label, lv_color_hex(BG_COLOR), 0);
  lv_obj_set_style_bg_opa(temp_label, LV_OPA_COVER, 0);

  week_label = lv_label_create(lv_scr_act());
  lv_label_set_text(week_label, "Mon.");
  lv_obj_add_style(week_label, &style_week, 0);
  lv_obj_set_pos(week_label, 90, 2);
  lv_obj_set_size(week_label, 60, 18);

  monthday_label = lv_label_create(lv_scr_act());
  lv_label_set_text(monthday_label, "1.1");
  lv_obj_add_style(monthday_label, &style_monthday, 0);
  lv_obj_set_pos(monthday_label, 4, 60);
  lv_obj_set_size(monthday_label, 80, 18);

  battery_label = NULL;

  lv_obj_t *logo = lv_img_create(lv_scr_act());
  lv_img_set_src(logo, &_WaveShare_alpha_25x25);
  lv_obj_set_pos(logo, 0, 209);

  gif_img = lv_animimg_create(lv_scr_act());
  if (gif_img != NULL && screen_GIF_Demo1_imgs != NULL) {
   
    lv_obj_set_pos(gif_img, 70, 58);
    lv_obj_set_size(gif_img, 50, 50);
    lv_animimg_set_src(gif_img, (const void **)screen_GIF_Demo1_imgs, 20);
    lv_animimg_set_duration(gif_img, 40 * 20);
    lv_animimg_set_repeat_count(gif_img, LV_ANIM_REPEAT_INFINITE);
    
    lv_obj_clear_flag(gif_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_opa(gif_img, LV_OPA_COVER, 0);
    
    lv_animimg_start(gif_img);
    
    lv_refr_now(NULL);
    
    Serial.println("GIF animation started");
    Serial.print("Animation object addr: ");
    Serial.println((uint32_t)gif_img, HEX);
    Serial.print("Image array addr: ");
    Serial.println((uint32_t)screen_GIF_Demo1_imgs, HEX);
  } else {
    Serial.println("Error: GIF animation object or image array is null");
    Serial.print("gif_img: ");
    Serial.println((uint32_t)gif_img, HEX);
    Serial.print("screen_GIF_Demo1_imgs: ");
    Serial.println((uint32_t)screen_GIF_Demo1_imgs, HEX);
  }

  bool allEmpty = true;
  for (int i = 0; i < 5; i++) {
    if (scrollText[i].length() > 0) {
        allEmpty = false;
        break;
    }
  }
  if (allEmpty) {
    scrollText[0] = "get weather...";
  }
  currentIndex = 0;
  prevTime = millis() - 2000;
  scrollBanner();
  currentIndex = 0;
  prevTime = 0;
  if (tip_label && scrollText[0].length() > 0) {
    lv_label_set_text(tip_label, scrollText[0].c_str());
  }
  clockScreenReady = true;
}

// 单独显示太空人GIF（不显示天气时钟UI）
void WeatherClock_ShowGifOnly(void) {
  lv_obj_clean(lv_scr_act());

  // 重置所有UI对象指针
  time_hm_label = NULL;
  time_s_label = NULL;
  week_label = NULL;
  monthday_label = NULL;
  city_label = NULL;
  temp_label = NULL;
  humidity_label = NULL;
  air_label = NULL;
  battery_label = NULL;
  tip_label = NULL;
  wind_label = NULL;
  follower_label = NULL;
  now_label = NULL;
  min_temp_label = NULL;
  weather_icon_img = NULL;
  temp_icon_img = NULL;
  humidity_icon_img = NULL;
  line_top = NULL;
  line_mid = NULL;
  line_bottom = NULL;
  line_vertical = NULL;
  status_label = NULL;
  hint_label = NULL;

  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(BG_COLOR), 0);
  lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
  lv_obj_set_size(lv_scr_act(), LCD_WIDTH, LCD_HEIGHT);
  lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

  gif_img = lv_animimg_create(lv_scr_act());
  if (gif_img != NULL && screen_GIF_Demo1_imgs != NULL) {
    lv_obj_center(gif_img);
    lv_obj_set_size(gif_img,128, 128);
    lv_animimg_set_src(gif_img, (const void **)screen_GIF_Demo1_imgs, 20);
    lv_animimg_set_duration(gif_img, 40 * 20);
    lv_animimg_set_repeat_count(gif_img, LV_ANIM_REPEAT_INFINITE);
    lv_obj_clear_flag(gif_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_opa(gif_img, LV_OPA_COVER, 0);
    lv_animimg_start(gif_img);
    Serial.println("GIF-only mode started");
  } else {
    Serial.println("Error: GIF animation failed to create");
  }
}

// 初始化天气时钟界面
void WeatherClock_Init(void) {
  static bool resourcesInitialized = false;

  clockScreenReady = false;
  cityCodeInitialized = false;
  initialWeatherReady = false;

  showStatusScreen(
    "Initializing the",
    "weather clock"
  );
  lv_timer_handler();

  get_wifi();
  if (WiFi.status() != WL_CONNECTED) {
    if (!wifiTimedOut && status_label) {
      lv_label_set_text(status_label, "WiFi not connect!");
    }
    return;
  }

  getCityCode();

  if (!initialWeatherReady) {
    currentIndex = 0;
    prevTime = millis() - 2000;
  }

  if (!resourcesInitialized) {
    Udp.begin(localPort);

    weatherRequestSem = xSemaphoreCreateBinary();
    if (weatherRequestSem != NULL) {
      xTaskCreate(
        weatherTask,
        "WeatherTask",
        8192,
        NULL,
        1,
        &weatherTaskHandle
      );
      Serial.println("Weather update background task created");
    } else {
      Serial.println("Warning: failed to create weather update semaphore");
    }

    Serial.println("Starting NTP time sync...");
    setSyncProvider(getNtpTime);
    setSyncInterval(setNTPSyncTime * 60);

    time_t syncTime = getNtpTime();
    if (syncTime > 0) {
      setTime(syncTime);
      Serial.print("Time set to: ");
      Serial.println(syncTime);
    } else {
      Serial.println("Warning: first NTP sync failed, will retry in background");
    }

    resourcesInitialized = true;
  }

  buildClockScreen();

  fanspush();
  if (weatherRequestSem != NULL) {
    requestWeatherUpdate();
  }
}

// 更新电池电量（已禁用，不再显示）
void updateBatteryLevel() {
}

// 请求天气更新（由主循环调用，触发后台任务）
void requestWeatherUpdate(void) {
  if (weatherRequestSem != NULL) {
    xSemaphoreGive(weatherRequestSem);
  }
}

// 天气更新后台任务（FreeRTOS）
void weatherTask(void *parameter) {
  while (1) {
    if (xSemaphoreTake(weatherRequestSem, portMAX_DELAY) == pdTRUE) {
      if (videoModeActive) {
        continue;
      }
      if (WiFi.status() == WL_CONNECTED) {
        if (!cityCodeInitialized) {
          Serial.println("Background task: first city code fetch");
          getCityCode(false);
          if (cityCodeInitialized) {
            Serial.println("Background task: starting weather fetch");
            getCityWeater();
          }
        } else {
          Serial.println("Background task: starting weather fetch");
          getCityWeater();
        }
        Serial.println("Background task: weather fetch complete");
      } else {
        Serial.println("Background task: WiFi not connected, skipping weather update");
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// 更新天气时钟
void WeatherClock_Update(void) {
  if (!clockScreenReady) {
    return;
  }
  digitalClockDisplay();
  
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - weaterTime > 300000) {
      weaterTime = millis();
      requestWeatherUpdate();
    }
  }
  
  weatherWarning();
  scrollBanner();
  updateBatteryLevel();
  
  if (millis() - backlightTime > 60000) {
    backlightTime = millis();
    backlightControl();
  }
}

// 主循环
void WeatherClock_Loop(void) {
  WeatherClock_Update();
}