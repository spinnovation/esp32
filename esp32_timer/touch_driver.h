#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include <Arduino.h>
#include <lvgl.h>

void init_touch();
void touch_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

#endif // TOUCH_DRIVER_H
