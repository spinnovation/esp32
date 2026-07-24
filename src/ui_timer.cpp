#include "ui_timer.h"
#include "timer_logic.h"
#include "config.h"

static lv_obj_t* eye_left;
static lv_obj_t* eye_right;
static lv_obj_t* label_timer;
static lv_obj_t* label_notice;
static lv_timer_t* ui_refresh_timer;

static uint8_t eye_anim_state = 0;
static uint16_t eye_timer_count = 0;

// Animated Cute Robot Eyes
static void update_robot_eyes() {
    eye_timer_count++;

    // Normal Blinking Every 3 seconds
    if (eye_timer_count % 15 == 0) {
        // Squish eyes for blink
        lv_obj_set_size(eye_left, 48, 8);
        lv_obj_set_size(eye_right, 48, 8);
        lv_obj_set_style_radius(eye_left, 4, 0);
        lv_obj_set_style_radius(eye_right, 4, 0);
    } else {
        TimerState st = g_timer.get_state();
        if (st == TIMER_RUNNING) {
            // Happy / Focused Eyes
            lv_obj_set_size(eye_left, 52, 45);
            lv_obj_set_size(eye_right, 52, 45);
            lv_obj_set_style_radius(eye_left, 24, 0);
            lv_obj_set_style_radius(eye_right, 24, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0); // Neon Cyan
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        } else if (st == TIMER_FINISHED) {
            // Joyful / Celebrating Eyes
            lv_obj_set_size(eye_left, 56, 56);
            lv_obj_set_size(eye_right, 56, 56);
            lv_obj_set_style_radius(eye_left, 28, 0);
            lv_obj_set_style_radius(eye_right, 28, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0xFF007F), 0); // Neon Pink
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0xFF007F), 0);
        } else {
            // Cute Playful Default Eyes
            lv_obj_set_size(eye_left, 48, 48);
            lv_obj_set_size(eye_right, 48, 48);
            lv_obj_set_style_radius(eye_left, 24, 0);
            lv_obj_set_style_radius(eye_right, 24, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        }
    }
}

static void ui_update_cb(lv_timer_t* timer) {
    g_timer.update();

    // Update Cute Robot Eyes Animation
    update_robot_eyes();

    // Update 80% Massive Timer Number
    char time_buf[16];
    g_timer.get_formatted_time(time_buf, sizeof(time_buf));
    lv_label_set_text(label_timer, time_buf);

    // Update Borderless Bottom Message
    lv_label_set_text(label_notice, g_timer.get_message());
}

void ui_timer_init() {
    lv_obj_t* scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0B0D14), 0); // Midnight Dark background
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // ==========================================
    // 1. TOP SECTION: Cute Playful Animated Robot Eyes (~130px)
    // ==========================================
    lv_obj_t* top_box = lv_obj_create(scr);
    lv_obj_set_size(top_box, 440, 120);
    lv_obj_align(top_box, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_opa(top_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(top_box, 0, 0);
    lv_obj_clear_flag(top_box, LV_OBJ_FLAG_SCROLLABLE);

    // Left Eye
    eye_left = lv_obj_create(top_box);
    lv_obj_set_size(eye_left, 48, 48);
    lv_obj_align(eye_left, LV_ALIGN_CENTER, -45, 0);
    lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(eye_left, 24, 0);
    lv_obj_set_style_border_width(eye_left, 0, 0);
    lv_obj_clear_flag(eye_left, LV_OBJ_FLAG_SCROLLABLE);

    // Right Eye
    eye_right = lv_obj_create(top_box);
    lv_obj_set_size(eye_right, 48, 48);
    lv_obj_align(eye_right, LV_ALIGN_CENTER, 45, 0);
    lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(eye_right, 24, 0);
    lv_obj_set_style_border_width(eye_right, 0, 0);
    lv_obj_clear_flag(eye_right, LV_OBJ_FLAG_SCROLLABLE);

    // ==========================================
    // DIVIDER LINE 1 (Top / Middle)
    // ==========================================
    static lv_point_t line1_points[] = { {20, 135}, {460, 135} };
    lv_obj_t* line1 = lv_line_create(scr);
    lv_line_set_points(line1, line1_points, 2);
    lv_obj_set_style_line_color(line1, lv_color_hex(0x1E2235), 0);
    lv_obj_set_style_line_width(line1, 2, 0);

    // ==========================================
    // 2. MIDDLE SECTION: Massive 80% Countdown Number (~240px)
    // ==========================================
    label_timer = lv_label_create(scr);
    lv_obj_set_style_text_color(label_timer, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_timer, &lv_font_montserrat_48, 0);
    lv_obj_set_style_transform_zoom(label_timer, 420, 0); // Scale 80% massive size
    lv_obj_align(label_timer, LV_ALIGN_CENTER, 0, -5);

    // ==========================================
    // DIVIDER LINE 2 (Middle / Bottom)
    // ==========================================
    static lv_point_t line2_points[] = { {20, 385}, {460, 385} };
    lv_obj_t* line2 = lv_line_create(scr);
    lv_line_set_points(line2, line2_points, 2);
    lv_obj_set_style_line_color(line2, lv_color_hex(0x1E2235), 0);
    lv_obj_set_style_line_width(line2, 2, 0);

    // ==========================================
    // 3. BOTTOM SECTION: Clean Borderless Status Text (~80px)
    // ==========================================
    label_notice = lv_label_create(scr);
    lv_obj_set_style_text_color(label_notice, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_text_font(label_notice, &lv_font_montserrat_20, 0);
    lv_obj_align(label_notice, LV_ALIGN_BOTTOM_MID, 0, -28);
    lv_label_set_text(label_notice, "WELCOME SEUNGPIL!");

    // Refresh UI every 200ms
    ui_refresh_timer = lv_timer_create(ui_update_cb, 200, NULL);
}

void ui_timer_update() {
    // Handled by ui_refresh_timer
}
