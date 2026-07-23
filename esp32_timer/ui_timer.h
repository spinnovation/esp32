#ifndef UI_TIMER_H
#define UI_TIMER_H

#include <lvgl.h>

void ui_timer_init();
void ui_timer_update();
void ui_clock_set_time(const char* time_str);

#endif // UI_TIMER_H
