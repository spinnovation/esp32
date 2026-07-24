#include "touch_driver.h"
#include "config.h"
#include <Wire.h>
#include <Touch_GT911.h>

// Guition ESP32-4848S040 Pinout per User Specification:
// SDA: 18, SCL: 17, INT: 38, RST: 38 (or -1)
#define TOUCH_GT911_SDA 18
#define TOUCH_GT911_SCL 17
#define TOUCH_GT911_INT 38
#define TOUCH_GT911_RST -1

static Touch_GT911 ts = Touch_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, LCD_WIDTH, LCD_HEIGHT);
static int touch_last_x = 0;
static int touch_last_y = 0;

void init_touch() {
    Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    delay(100);

    ts.begin();
    ts.setRotation(ROTATION_NORMAL);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    lv_indev_drv_register(&indev_drv);
    Serial.println("GT911 Touch initialized on SDA: 18, SCL: 17, INT: 38.");
}

void touch_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    ts.read();
    if (ts.isTouched) {
        touch_last_x = map(ts.points[0].x, 480, 0, 0, LCD_WIDTH - 1);
        touch_last_y = map(ts.points[0].y, 480, 0, 0, LCD_HEIGHT - 1);

        data->state = LV_INDEV_STATE_PR;
        data->point.x = constrain(touch_last_x, 0, LCD_WIDTH - 1);
        data->point.y = constrain(touch_last_y, 0, LCD_HEIGHT - 1);
        Serial.printf("[GT911 TOUCH] Press at X: %d, Y: %d\n", data->point.x, data->point.y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
