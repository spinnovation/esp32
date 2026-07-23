#include "touch_driver.h"
#include "config.h"
#include <Wire.h>
#include <TAMC_GT911.h>

// Exact official vendor initialization from 4.0_LvglWidgets/touch.h
static TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, 480, 480);
static int touch_last_x = 0;
static int touch_last_y = 0;

void init_touch() {
    Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    ts.begin();
    ts.setRotation(ROTATION_NORMAL);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_cb;
    lv_indev_drv_register(&indev_drv);
}

void touch_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    ts.read();
    if (ts.isTouched) {
        // Official vendor coordinate mapping from touch.h
        touch_last_x = map(ts.points[0].x, 480, 0, 0, LCD_WIDTH - 1);
        touch_last_y = map(ts.points[0].y, 480, 0, 0, LCD_HEIGHT - 1);

        data->state = LV_INDEV_STATE_PR;
        data->point.x = constrain(touch_last_x, 0, LCD_WIDTH - 1);
        data->point.y = constrain(touch_last_y, 0, LCD_HEIGHT - 1);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
