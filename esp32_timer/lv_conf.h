#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

#define LV_COLOR_16_SWAP 0
#define LV_COLOR_DEPTH 16
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

/* Critical LVGL Tick Source from Arduino millis() */
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

/* Memory manager */
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (64 * 1024U)

/* HAL settings */
#define LV_DISP_DEF_REFR_PERIOD 16
#define LV_INDEV_DEF_READ_PERIOD 16

/* Feature selection */
#define LV_USE_ARC 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_LABEL 1
#define LV_USE_LINE 1
#define LV_USE_SLIDER 1
#define LV_USE_SPINNER 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 1

/* Extra Widgets */
#define LV_USE_ANIMIMG 1
#define LV_USE_CALENDAR 1
#define LV_USE_CHART 1
#define LV_USE_COLORWHEEL 1
#define LV_USE_IMGBTN 1
#define LV_USE_KEYBOARD 1
#define LV_USE_LED 1
#define LV_USE_METER 1
#define LV_USE_MSGBOX 1
#define LV_USE_SPINBOX 1
#define LV_USE_TABVIEW 1
#define LV_USE_TILEVIEW 1
#define LV_USE_WIN 1

/* Layouts */
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/* Fonts */
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_48 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

#endif /* LV_CONF_H */
