#include "ui_timer.h"
#include "timer_logic.h"
#include "config.h"

static lv_obj_t* label_clock;
static lv_obj_t* label_date;
static lv_obj_t* arc_progress;
static lv_obj_t* label_timer;
static lv_obj_t* label_notice;
static lv_timer_t* ui_refresh_timer;

static void update_clock_display() {
    unsigned long sec = millis() / 1000;
    uint32_t hours = (11 + (sec / 3600)) % 24;
    uint32_t mins = (sec / 60) % 60;
    uint32_t secs = sec % 60;

    char time_str[32];
    snprintf(time_str, sizeof(time_str), "%02u:%02u:%02u", hours, mins, secs);
    lv_label_set_text(label_clock, time_str);
    lv_label_set_text(label_date, "2026-07-24 | ONKI STUDIO");
}

static void ui_update_cb(lv_timer_t* timer) {
    g_timer.update();

    // Update Top Section Clock
    update_clock_display();

    // Update Middle Section Timer
    char time_buf[16];
    g_timer.get_formatted_time(time_buf, sizeof(time_buf));
    lv_label_set_text(label_timer, time_buf);

    int16_t arc_val = (int16_t)(g_timer.get_progress() * 100.0f);
    lv_arc_set_value(arc_progress, arc_val);

    // Update Bottom Section Message
    lv_label_set_text(label_notice, g_timer.get_message());
}

void ui_timer_init() {
    lv_obj_t* scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0F111A), 0); // Midnight Dark background
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // ==========================================
    // 1. TOP SECTION (Clock & Date) - Height 120px
    // ==========================================
    lv_obj_t* top_card = lv_obj_create(scr);
    lv_obj_set_size(top_card, 440, 110);
    lv_obj_align(top_card, LV_ALIGN_TOP_MID, 0, 15);
    lv_obj_set_style_bg_color(top_card, lv_color_hex(0x181B28), 0);
    lv_obj_set_style_border_color(top_card, lv_color_hex(0x282C40), 0);
    lv_obj_set_style_border_width(top_card, 1, 0);
    lv_obj_set_style_radius(top_card, 16, 0);
    lv_obj_clear_flag(top_card, LV_OBJ_FLAG_SCROLLABLE);

    label_clock = lv_label_create(top_card);
    lv_obj_set_style_text_color(label_clock, lv_color_hex(0x00E5FF), 0); // Neon Cyan
    lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_32, 0);
    lv_obj_align(label_clock, LV_ALIGN_TOP_MID, 0, 8);

    label_date = lv_label_create(top_card);
    lv_obj_set_style_text_color(label_date, lv_color_hex(0x8F9BB3), 0);
    lv_obj_set_style_text_font(label_date, &lv_font_montserrat_14, 0);
    lv_obj_align(label_date, LV_ALIGN_BOTTOM_MID, 0, -8);

    // ==========================================
    // 2. MIDDLE SECTION (50-Min Timer Arc & Text) - Height 230px
    // ==========================================
    arc_progress = lv_arc_create(scr);
    lv_obj_set_size(arc_progress, 210, 210);
    lv_obj_align(arc_progress, LV_ALIGN_CENTER, 0, -10);
    lv_arc_set_rotation(arc_progress, 270);
    lv_arc_set_bg_angles(arc_progress, 0, 360);
    lv_arc_set_range(arc_progress, 0, 100);
    lv_arc_set_value(arc_progress, 0);
    lv_obj_remove_style(arc_progress, NULL, LV_PART_KNOB);

    // Dark Arc Track
    lv_obj_set_style_arc_color(arc_progress, lv_color_hex(0x222638), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_progress, 14, LV_PART_MAIN);

    // Neon Cyan Arc Progress Indicator
    lv_obj_set_style_arc_color(arc_progress, lv_color_hex(0x00E5FF), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_progress, 14, LV_PART_INDICATOR);

    label_timer = lv_label_create(scr);
    lv_obj_set_style_text_color(label_timer, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_timer, &lv_font_montserrat_48, 0);
    lv_obj_align(label_timer, LV_ALIGN_CENTER, 0, -10);

    // ==========================================
    // 3. BOTTOM SECTION (Status Notice Banner) - Height 100px
    // ==========================================
    lv_obj_t* bottom_card = lv_obj_create(scr);
    lv_obj_set_size(bottom_card, 440, 95);
    lv_obj_align(bottom_card, LV_ALIGN_BOTTOM_MID, 0, -15);
    lv_obj_set_style_bg_color(bottom_card, lv_color_hex(0x181B28), 0);
    lv_obj_set_style_border_color(bottom_card, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(bottom_card, 1, 0);
    lv_obj_set_style_radius(bottom_card, 16, 0);
    lv_obj_clear_flag(bottom_card, LV_OBJ_FLAG_SCROLLABLE);

    label_notice = lv_label_create(bottom_card);
    lv_obj_set_style_text_color(label_notice, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_text_font(label_notice, &lv_font_montserrat_20, 0);
    lv_obj_align(label_notice, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label_notice, "WELCOME SEUNGPIL!");

    // Refresh UI every 200ms
    ui_refresh_timer = lv_timer_create(ui_update_cb, 200, NULL);
}

void ui_timer_update() {
    // Handled by ui_refresh_timer
}
