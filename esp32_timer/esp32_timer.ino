#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "display_driver.h"
#include "touch_driver.h"
#include "timer_logic.h"
#include "ui_timer.h"

// Wi-Fi Configuration
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* NTP_SERVER = "pool.ntp.org";
const long  GMT_OFFSET_SEC = 9 * 3600; // KST (UTC+9)
const int   DAYLIGHT_OFFSET_SEC = 0;

static unsigned long last_clock_update_ms = 0;

void setup_wifi_and_time() {
    if (strcmp(WIFI_SSID, "YOUR_WIFI_SSID") != 0) {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        int retry = 0;
        while (WiFi.status() != WL_CONNECTED && retry < 10) {
            delay(500);
            retry++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
        }
    }
}

void update_clock_display() {
    unsigned long now = millis();
    if (now - last_clock_update_ms >= 1000) {
        last_clock_update_ms = now;

        struct tm timeinfo;
        char time_str[32];

        if (getLocalTime(&timeinfo)) {
            strftime(time_str, sizeof(time_str), "%I:%M:%S %p", &timeinfo);
        } else {
            uint32_t total_sec = now / 1000;
            uint32_t hh = (total_sec / 3600) % 24;
            uint32_t mm = (total_sec / 60) % 60;
            uint32_t ss = total_sec % 60;
            snprintf(time_str, sizeof(time_str), "%02u:%02u:%02u", hh, mm, ss);
        }

        ui_clock_set_time(time_str);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting ESP32-S3 4.0\" LVGL 50-Min Timer & Clock...");

    // Initialize LVGL core
    lv_init();

    // CRITICAL: Initialize Touch BEFORE Display per official vendor specification
    init_touch();
    init_display();

    // Initialize 50-Minute Timer Logic (3000 seconds)
    g_timer.begin(DEFAULT_TIMER_SECONDS);

    // Initialize UI
    ui_timer_init();
    ui_timer_update();

    // Setup Wi-Fi & NTP Time
    setup_wifi_and_time();

    Serial.println("Initialization complete.");
}

void loop() {
    // 1. Advance LVGL tick time so touch timers & event callbacks advance
    lv_tick_inc(5);

    // 2. Update LVGL GUI tasks
    lv_timer_handler();

    // 3. Update Timer countdown & logic
    g_timer.update();
    ui_timer_update();

    // 4. Update top digital clock
    update_clock_display();

    delay(5);
}
