#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include "display_driver.h"
#include "timer_logic.h"
#include "ui_timer.h"

void handle_serial_commands() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd.length() == 0) return;

        Serial.printf("[MAC CMD RECEIVED] %s\n", cmd.c_str());

        if (cmd.equalsIgnoreCase("START") || cmd.equalsIgnoreCase("S")) {
            g_timer.start();
        } else if (cmd.equalsIgnoreCase("PAUSE") || cmd.equalsIgnoreCase("P")) {
            g_timer.pause();
        } else if (cmd.equalsIgnoreCase("RESET") || cmd.equalsIgnoreCase("R")) {
            g_timer.reset();
        } else if (cmd.startsWith("MSG:")) {
            String msg = cmd.substring(4);
            g_timer.set_message(msg.c_str());
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("Starting ESP32-S3 3-Section Monitor Display...");

    // Initialize LVGL Graphics Engine
    lv_init();

    // Initialize ST7701S RGB Display (Rotation 3: upright)
    init_display();

    // Initialize 50-Min Timer & 3-Section Monitor UI
    g_timer.begin(3000);
    ui_timer_init();

    Serial.println("3-Section Monitor Ready! Control via Mac USB Serial Commands (START/PAUSE/RESET/MSG:text).");
}

void loop() {
    handle_serial_commands();
    lv_timer_handler();
    delay(5);
}
