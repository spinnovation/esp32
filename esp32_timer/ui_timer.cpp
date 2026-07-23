#include "ui_timer.h"
#include "timer_logic.h"
#include "config.h"
#include <stdio.h>

static lv_obj_t *lbl_clock;
static lv_obj_t *arc_progress;
static lv_obj_t *lbl_timer_val;
static lv_obj_t *lbl_timer_status;
static lv_obj_t *btn_start_pause;
static lv_obj_t *lbl_btn_start;
static lv_obj_t *btn_reset;

// Start / Pause Callback for ALL click / touch events
static void btn_start_pause_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_PRESSED || code == LV_EVENT_SHORT_CLICKED) {
        Serial.println("[UI EVENT] START/PAUSE Button Pressed!");
        g_timer.toggle();
        ui_timer_update();
    }
}

// Reset Callback
static void btn_reset_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_PRESSED || code == LV_EVENT_SHORT_CLICKED) {
        Serial.println("[UI EVENT] RESET Button Pressed!");
        g_timer.reset();
        ui_timer_update();
    }
}

// Full screen touch fallback callback
static void screen_click_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED || code == LV_EVENT_PRESSED) {
        lv_point_t p;
        lv_indev_get_point(lv_indev_get_act(), &p);
        Serial.printf("[SCREEN TOUCH] Pressed at X: %d, Y: %d\n", p.x, p.y);
        
        // If pressed on left side (START button area), toggle timer
        if (p.x < 240 && p.y > 350) {
            g_timer.toggle();
            ui_timer_update();
        } 
        // If pressed on right side (RESET button area), reset timer
        else if (p.x >= 240 && p.y > 350) {
            g_timer.reset();
            ui_timer_update();
        }
        // If pressed inside center ring area, toggle timer
        else if (p.y >= 80 && p.y <= 380) {
            g_timer.toggle();
            ui_timer_update();
        }
    }
}

void ui_timer_init() {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x12131C), 0); // Premium dark background
    lv_obj_add_flag(scr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(scr, screen_click_cb, LV_EVENT_ALL, NULL);

    // 1. Top Header Bar: Digital Clock
    lbl_clock = lv_label_create(scr);
    lv_obj_align(lbl_clock, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_color(lbl_clock, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_text_font(lbl_clock, &lv_font_montserrat_24, 0);
    lv_label_set_text(lbl_clock, "12:00:00 PM");

    // 2. Main Circular Progress Arc (Centered 340x340)
    arc_progress = lv_arc_create(scr);
    lv_obj_set_size(arc_progress, 340, 340);
    lv_obj_align(arc_progress, LV_ALIGN_CENTER, 0, -25);
    lv_arc_set_rotation(arc_progress, 270);
    lv_arc_set_bg_angles(arc_progress, 0, 360);
    lv_arc_set_range(arc_progress, 0, 1000);
    lv_arc_set_value(arc_progress, 1000);
    
    // Style Arc
    lv_obj_set_style_arc_color(arc_progress, lv_color_hex(0x222638), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_progress, 18, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_progress, lv_color_hex(0x00E5FF), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_progress, 18, LV_PART_INDICATOR);
    lv_obj_remove_style(arc_progress, NULL, LV_PART_KNOB);

    // Timer Value Label inside Arc
    lbl_timer_val = lv_label_create(scr);
    lv_obj_align(lbl_timer_val, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_text_color(lbl_timer_val, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl_timer_val, &lv_font_montserrat_48, 0);
    lv_label_set_text(lbl_timer_val, "50:00");

    // Timer Status Label inside Arc
    lbl_timer_status = lv_label_create(scr);
    lv_obj_align(lbl_timer_status, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_text_color(lbl_timer_status, lv_color_hex(0x8892B0), 0);
    lv_obj_set_style_text_font(lbl_timer_status, &lv_font_montserrat_16, 0);
    lv_label_set_text(lbl_timer_status, "50 MIN FOCUS");

    // 3. Action Buttons (START / PAUSE, RESET)
    btn_start_pause = lv_btn_create(scr);
    lv_obj_set_size(btn_start_pause, 190, 56);
    lv_obj_align(btn_start_pause, LV_ALIGN_BOTTOM_LEFT, 35, -25);
    lv_obj_set_style_bg_color(btn_start_pause, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(btn_start_pause, 28, 0);
    lv_obj_add_flag(btn_start_pause, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(btn_start_pause, btn_start_pause_cb, LV_EVENT_ALL, NULL);

    lbl_btn_start = lv_label_create(btn_start_pause);
    lv_label_set_text(lbl_btn_start, "START");
    lv_obj_center(lbl_btn_start);
    lv_obj_set_style_text_font(lbl_btn_start, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_btn_start, lv_color_hex(0x0A192F), 0);

    btn_reset = lv_btn_create(scr);
    lv_obj_set_size(btn_reset, 180, 56);
    lv_obj_align(btn_reset, LV_ALIGN_BOTTOM_RIGHT, -35, -25);
    lv_obj_set_style_bg_color(btn_reset, lv_color_hex(0x232B42), 0);
    lv_obj_set_style_radius(btn_reset, 28, 0);
    lv_obj_add_flag(btn_reset, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(btn_reset, btn_reset_cb, LV_EVENT_ALL, NULL);

    lv_obj_t *lbl_btn_reset = lv_label_create(btn_reset);
    lv_label_set_text(lbl_btn_reset, "RESET");
    lv_obj_center(lbl_btn_reset);
    lv_obj_set_style_text_font(lbl_btn_reset, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_btn_reset, lv_color_hex(0x8892B0), 0);
}

void ui_clock_set_time(const char* time_str) {
    if (lbl_clock) {
        lv_label_set_text(lbl_clock, time_str);
    }
}

void ui_timer_update() {
    uint32_t rem_sec = g_timer.get_remaining_seconds();
    uint32_t total_sec = g_timer.get_total_seconds();

    uint32_t mm = rem_sec / 60;
    uint32_t ss = rem_sec % 60;

    char buf[16];
    snprintf(buf, sizeof(buf), "%02u:%02u", mm, ss);
    if (lbl_timer_val) {
        lv_label_set_text(lbl_timer_val, buf);
    }

    if (arc_progress && total_sec > 0) {
        uint32_t arc_val = (rem_sec * 1000) / total_sec;
        lv_arc_set_value(arc_progress, arc_val);
    }

    if (g_timer.get_state() == TIMER_RUNNING) {
        if (lbl_btn_start) lv_label_set_text(lbl_btn_start, "PAUSE");
        if (btn_start_pause) lv_obj_set_style_bg_color(btn_start_pause, lv_color_hex(0xFF9800), 0);
        if (lbl_timer_status) lv_label_set_text(lbl_timer_status, "COUNTING DOWN");
    } else {
        if (lbl_btn_start) lv_label_set_text(lbl_btn_start, "START");
        if (btn_start_pause) lv_obj_set_style_bg_color(btn_start_pause, lv_color_hex(0x00E5FF), 0);
        if (lbl_timer_status) {
            if (g_timer.get_state() == TIMER_FINISHED) {
                lv_label_set_text(lbl_timer_status, "TIME COMPLETE!");
            } else {
                lv_label_set_text(lbl_timer_status, "50 MIN FOCUS");
            }
        }
    }
}
