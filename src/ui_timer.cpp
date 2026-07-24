#include "ui_timer.h"
#include "timer_logic.h"
#include "config.h"

static lv_obj_t* eye_left;
static lv_obj_t* eye_right;
static lv_obj_t* tear_left;
static lv_obj_t* tear_right;

// 5 Digit objects for 120px Bold Digital Timer Display
static lv_obj_t* digit_segs[4][7]; // 4 digits (M1, M2, S1, S2), 7 segments each
static lv_obj_t* colon_dot1;
static lv_obj_t* colon_dot2;

static lv_obj_t* label_notice;
static lv_timer_t* ui_refresh_timer;

static uint16_t anim_tick = 0;
static uint8_t random_expr = 0;

// Segment Lookup Table for Digits 0-9
static const bool seg_map[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

// Create a 120px tall 7-segment bold digital digit
static void create_digital_digit(lv_obj_t* parent, int idx, int x_center) {
    int w = 62;
    int h = 130;
    int t = 12; // Segment thickness 12px

    int half_h = h / 2;

    // Segment A (Top horizontal)
    digit_segs[idx][0] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][0], w - 4, t);
    lv_obj_align(digit_segs[idx][0], LV_ALIGN_CENTER, x_center, -half_h);

    // Segment B (Top-right vertical)
    digit_segs[idx][1] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][1], t, half_h - 2);
    lv_obj_align(digit_segs[idx][1], LV_ALIGN_CENTER, x_center + (w/2) - (t/2), -half_h/2);

    // Segment C (Bottom-right vertical)
    digit_segs[idx][2] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][2], t, half_h - 2);
    lv_obj_align(digit_segs[idx][2], LV_ALIGN_CENTER, x_center + (w/2) - (t/2), half_h/2);

    // Segment D (Bottom horizontal)
    digit_segs[idx][3] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][3], w - 4, t);
    lv_obj_align(digit_segs[idx][3], LV_ALIGN_CENTER, x_center, half_h);

    // Segment E (Bottom-left vertical)
    digit_segs[idx][4] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][4], t, half_h - 2);
    lv_obj_align(digit_segs[idx][4], LV_ALIGN_CENTER, x_center - (w/2) + (t/2), half_h/2);

    // Segment F (Top-left vertical)
    digit_segs[idx][5] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][5], t, half_h - 2);
    lv_obj_align(digit_segs[idx][5], LV_ALIGN_CENTER, x_center - (w/2) + (t/2), -half_h/2);

    // Segment G (Middle horizontal)
    digit_segs[idx][6] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][6], w - 4, t);
    lv_obj_align(digit_segs[idx][6], LV_ALIGN_CENTER, x_center, 0);

    // Styling for all 7 segments
    for (int s = 0; s < 7; s++) {
        lv_obj_set_style_radius(digit_segs[idx][s], 6, 0);
        lv_obj_set_style_border_width(digit_segs[idx][s], 0, 0);
        lv_obj_clear_flag(digit_segs[idx][s], LV_OBJ_FLAG_SCROLLABLE);
    }
}

static void update_digit_value(int idx, int val) {
    if (val < 0 || val > 9) return;
    for (int s = 0; s < 7; s++) {
        if (seg_map[val][s]) {
            lv_obj_set_style_bg_color(digit_segs[idx][s], lv_color_hex(0xFFFFFF), 0); // Active White
            lv_obj_set_style_bg_opa(digit_segs[idx][s], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_color(digit_segs[idx][s], lv_color_hex(0x161A26), 0); // Dim Dark
            lv_obj_set_style_bg_opa(digit_segs[idx][s], LV_OPA_COVER, 0);
        }
    }
}

// Animated Robot Eyes
static void update_robot_eyes() {
    anim_tick++;
    TimerState st = g_timer.get_state();

    lv_obj_add_flag(tear_left, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tear_right, LV_OBJ_FLAG_HIDDEN);

    if (st == TIMER_RUNNING) {
        lv_obj_set_size(eye_left, 56, 40);
        lv_obj_set_size(eye_right, 56, 40);
        lv_obj_set_style_radius(eye_left, 20, 0);
        lv_obj_set_style_radius(eye_right, 20, 0);
        lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
        lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);

        if ((anim_tick / 5) % 2 == 0) {
            lv_obj_set_size(eye_left, 56, 44);
            lv_obj_set_size(eye_right, 56, 44);
        }
    } else if (st == TIMER_PAUSED) {
        lv_obj_set_size(eye_left, 44, 12);
        lv_obj_set_size(eye_right, 44, 12);
        lv_obj_set_style_radius(eye_left, 6, 0);
        lv_obj_set_style_radius(eye_right, 6, 0);
        lv_obj_set_style_bg_color(eye_left, lv_color_hex(0xFF3366), 0);
        lv_obj_set_style_bg_color(eye_right, lv_color_hex(0xFF3366), 0);

        lv_obj_clear_flag(tear_left, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(tear_right, LV_OBJ_FLAG_HIDDEN);

        int16_t drop_y = 15 + ((anim_tick * 4) % 25);
        lv_obj_align(tear_left, LV_ALIGN_CENTER, -45, drop_y);
        lv_obj_align(tear_right, LV_ALIGN_CENTER, 45, drop_y);
    } else if (st == TIMER_FINISHED) {
        lv_obj_set_size(eye_left, 60, 60);
        lv_obj_set_size(eye_right, 60, 60);
        lv_obj_set_style_radius(eye_left, 30, 0);
        lv_obj_set_style_radius(eye_right, 30, 0);

        if ((anim_tick / 3) % 2 == 0) {
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0xFFD700), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0xFFD700), 0);
        } else {
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0xFF007F), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0xFF007F), 0);
        }
    } else {
        if (anim_tick % 25 == 0) {
            random_expr = (random_expr + 1) % 4;
        }

        if (random_expr == 0) {
            lv_obj_set_size(eye_left, 48, 48);
            lv_obj_set_size(eye_right, 48, 48);
            lv_obj_set_style_radius(eye_left, 24, 0);
            lv_obj_set_style_radius(eye_right, 24, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        } else if (random_expr == 1) {
            lv_obj_set_size(eye_left, 48, 8);
            lv_obj_set_size(eye_right, 48, 48);
            lv_obj_set_style_radius(eye_left, 4, 0);
            lv_obj_set_style_radius(eye_right, 24, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        } else if (random_expr == 2) {
            lv_obj_set_size(eye_left, 48, 8);
            lv_obj_set_size(eye_right, 48, 8);
            lv_obj_set_style_radius(eye_left, 4, 0);
            lv_obj_set_style_radius(eye_right, 4, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        } else {
            lv_obj_set_size(eye_left, 52, 40);
            lv_obj_set_size(eye_right, 52, 40);
            lv_obj_set_style_radius(eye_left, 20, 0);
            lv_obj_set_style_radius(eye_right, 20, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00FF99), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00FF99), 0);
        }
    }
}

static void ui_update_cb(lv_timer_t* timer) {
    g_timer.update();

    update_robot_eyes();

    // Update 120px Bold Digital Segments
    uint32_t rem = g_timer.get_remaining_seconds();
    uint32_t mins = rem / 60;
    uint32_t secs = rem % 60;

    update_digit_value(0, mins / 10);
    update_digit_value(1, mins % 10);
    update_digit_value(2, secs / 10);
    update_digit_value(3, secs % 10);

    // Blink Colon Dot every second
    if ((anim_tick / 3) % 2 == 0) {
        lv_obj_set_style_bg_color(colon_dot1, lv_color_hex(0x00E5FF), 0);
        lv_obj_set_style_bg_color(colon_dot2, lv_color_hex(0x00E5FF), 0);
    } else {
        lv_obj_set_style_bg_color(colon_dot1, lv_color_hex(0x161A26), 0);
        lv_obj_set_style_bg_color(colon_dot2, lv_color_hex(0x161A26), 0);
    }

    lv_label_set_text(label_notice, g_timer.get_message());
}

void ui_timer_init() {
    lv_obj_t* scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0B0D14), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // ==========================================
    // 1. TOP SECTION: Expression Robot Eyes (~120px)
    // ==========================================
    lv_obj_t* top_box = lv_obj_create(scr);
    lv_obj_set_size(top_box, 440, 110);
    lv_obj_align(top_box, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_opa(top_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(top_box, 0, 0);
    lv_obj_clear_flag(top_box, LV_OBJ_FLAG_SCROLLABLE);

    eye_left = lv_obj_create(top_box);
    lv_obj_set_size(eye_left, 48, 48);
    lv_obj_align(eye_left, LV_ALIGN_CENTER, -45, 0);
    lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(eye_left, 24, 0);
    lv_obj_set_style_border_width(eye_left, 0, 0);
    lv_obj_clear_flag(eye_left, LV_OBJ_FLAG_SCROLLABLE);

    eye_right = lv_obj_create(top_box);
    lv_obj_set_size(eye_right, 48, 48);
    lv_obj_align(eye_right, LV_ALIGN_CENTER, 45, 0);
    lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(eye_right, 24, 0);
    lv_obj_set_style_border_width(eye_right, 0, 0);
    lv_obj_clear_flag(eye_right, LV_OBJ_FLAG_SCROLLABLE);

    tear_left = lv_obj_create(top_box);
    lv_obj_set_size(tear_left, 10, 18);
    lv_obj_set_style_bg_color(tear_left, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(tear_left, 5, 0);
    lv_obj_set_style_border_width(tear_left, 0, 0);
    lv_obj_add_flag(tear_left, LV_OBJ_FLAG_HIDDEN);

    tear_right = lv_obj_create(top_box);
    lv_obj_set_size(tear_right, 10, 18);
    lv_obj_set_style_bg_color(tear_right, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(tear_right, 5, 0);
    lv_obj_set_style_border_width(tear_right, 0, 0);
    lv_obj_add_flag(tear_right, LV_OBJ_FLAG_HIDDEN);

    // DIVIDER LINE 1
    static lv_point_t line1_points[] = { {20, 125}, {460, 125} };
    lv_obj_t* line1 = lv_line_create(scr);
    lv_line_set_points(line1, line1_points, 2);
    lv_obj_set_style_line_color(line1, lv_color_hex(0x1E2235), 0);
    lv_obj_set_style_line_width(line1, 2, 0);

    // ==========================================
    // 2. MIDDLE SECTION: 120px Vector Bold Digital Digits (80% Middle Space)
    // ==========================================
    lv_obj_t* timer_box = lv_obj_create(scr);
    lv_obj_set_size(timer_box, 460, 250);
    lv_obj_align(timer_box, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(timer_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(timer_box, 0, 0);
    lv_obj_clear_flag(timer_box, LV_OBJ_FLAG_SCROLLABLE);

    // Create 4 Digital Digits: Min1 (-150), Min2 (-60), Sec1 (+60), Sec2 (+150)
    create_digital_digit(timer_box, 0, -150);
    create_digital_digit(timer_box, 1, -60);
    create_digital_digit(timer_box, 2, 60);
    create_digital_digit(timer_box, 3, 150);

    // Create Colon Dots
    colon_dot1 = lv_obj_create(timer_box);
    lv_obj_set_size(colon_dot1, 14, 14);
    lv_obj_align(colon_dot1, LV_ALIGN_CENTER, 0, -28);
    lv_obj_set_style_radius(colon_dot1, 7, 0);
    lv_obj_set_style_border_width(colon_dot1, 0, 0);
    lv_obj_clear_flag(colon_dot1, LV_OBJ_FLAG_SCROLLABLE);

    colon_dot2 = lv_obj_create(timer_box);
    lv_obj_set_size(colon_dot2, 14, 14);
    lv_obj_align(colon_dot2, LV_ALIGN_CENTER, 0, 28);
    lv_obj_set_style_radius(colon_dot2, 7, 0);
    lv_obj_set_style_border_width(colon_dot2, 0, 0);
    lv_obj_clear_flag(colon_dot2, LV_OBJ_FLAG_SCROLLABLE);

    // DIVIDER LINE 2
    static lv_point_t line2_points[] = { {20, 395}, {460, 395} };
    lv_obj_t* line2 = lv_line_create(scr);
    lv_line_set_points(line2, line2_points, 2);
    lv_obj_set_style_line_color(line2, lv_color_hex(0x1E2235), 0);
    lv_obj_set_style_line_width(line2, 2, 0);

    // ==========================================
    // 3. BOTTOM SECTION: Clean Status Text
    // ==========================================
    label_notice = lv_label_create(scr);
    lv_obj_set_style_text_color(label_notice, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_text_font(label_notice, &lv_font_montserrat_20, 0);
    lv_obj_align(label_notice, LV_ALIGN_BOTTOM_MID, 0, -22);
    lv_label_set_text(label_notice, "WELCOME SEUNGPIL!");

    ui_refresh_timer = lv_timer_create(ui_update_cb, 200, NULL);
}

void ui_timer_update() {
    // Handled by ui_refresh_timer
}
