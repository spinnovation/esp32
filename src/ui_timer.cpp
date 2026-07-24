#include "ui_timer.h"
#include "timer_logic.h"
#include "config.h"

static lv_obj_t* eye_left;
static lv_obj_t* eye_right;
static lv_obj_t* tear_left;
static lv_obj_t* tear_right;

static lv_obj_t* label_min1;
static lv_obj_t* label_min2;
static lv_obj_t* label_colon;
static lv_obj_t* label_sec1;
static lv_obj_t* label_sec2;

static lv_obj_t* label_notice;
static lv_timer_t* ui_refresh_timer;

static uint16_t anim_tick = 0;
static uint8_t random_expr = 0;

// Update Robot Eyes Expressions
static void update_robot_eyes() {
    anim_tick++;

    TimerState st = g_timer.get_state();

    // Hide tears by default
    lv_obj_add_flag(tear_left, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tear_right, LV_OBJ_FLAG_HIDDEN);

    if (st == TIMER_RUNNING) {
        // FOCUS / DETERMINED EYES (> <)
        lv_obj_set_size(eye_left, 56, 40);
        lv_obj_set_size(eye_right, 56, 40);
        lv_obj_set_style_radius(eye_left, 20, 0);
        lv_obj_set_style_radius(eye_right, 20, 0);
        lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0); // Neon Cyan
        lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);

        // Subtle pulsing animation while running
        if ((anim_tick / 5) % 2 == 0) {
            lv_obj_set_size(eye_left, 56, 44);
            lv_obj_set_size(eye_right, 56, 44);
        }
    } else if (st == TIMER_PAUSED) {
        // SAD / CRYING EYES (T T)
        lv_obj_set_size(eye_left, 44, 12);
        lv_obj_set_size(eye_right, 44, 12);
        lv_obj_set_style_radius(eye_left, 6, 0);
        lv_obj_set_style_radius(eye_right, 6, 0);
        lv_obj_set_style_bg_color(eye_left, lv_color_hex(0xFF3366), 0); // Sad Red/Pink
        lv_obj_set_style_bg_color(eye_right, lv_color_hex(0xFF3366), 0);

        // Show crying teardrops
        lv_obj_clear_flag(tear_left, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(tear_right, LV_OBJ_FLAG_HIDDEN);

        // Animated teardrop falling
        int16_t drop_y = 15 + ((anim_tick * 4) % 25);
        lv_obj_align(tear_left, LV_ALIGN_CENTER, -45, drop_y);
        lv_obj_align(tear_right, LV_ALIGN_CENTER, 45, drop_y);
    } else if (st == TIMER_FINISHED) {
        // JOYFUL / LAUGHING CELEBRATION EYES (^ ^)
        lv_obj_set_size(eye_left, 60, 60);
        lv_obj_set_size(eye_right, 60, 60);
        lv_obj_set_style_radius(eye_left, 30, 0);
        lv_obj_set_style_radius(eye_right, 30, 0);

        // Pulsing Gold/Pink celebration colors
        if ((anim_tick / 3) % 2 == 0) {
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0xFFD700), 0); // Gold
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0xFFD700), 0);
        } else {
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0xFF007F), 0); // Neon Pink
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0xFF007F), 0);
        }
    } else {
        // IDLE PLAYFUL CYCLE (Blink, Wink, Look Around)
        if (anim_tick % 25 == 0) {
            random_expr = (random_expr + 1) % 4;
        }

        if (random_expr == 0) { // Normal Eye
            lv_obj_set_size(eye_left, 48, 48);
            lv_obj_set_size(eye_right, 48, 48);
            lv_obj_set_style_radius(eye_left, 24, 0);
            lv_obj_set_style_radius(eye_right, 24, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        } else if (random_expr == 1) { // WINK (Left eye closed, Right eye open)
            lv_obj_set_size(eye_left, 48, 8);
            lv_obj_set_size(eye_right, 48, 48);
            lv_obj_set_style_radius(eye_left, 4, 0);
            lv_obj_set_style_radius(eye_right, 24, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        } else if (random_expr == 2) { // BLINK (Both eyes closed)
            lv_obj_set_size(eye_left, 48, 8);
            lv_obj_set_size(eye_right, 48, 8);
            lv_obj_set_style_radius(eye_left, 4, 0);
            lv_obj_set_style_radius(eye_right, 4, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
        } else { // HAPPY SMILE EYES
            lv_obj_set_size(eye_left, 52, 40);
            lv_obj_set_size(eye_right, 52, 40);
            lv_obj_set_style_radius(eye_left, 20, 0);
            lv_obj_set_style_radius(eye_right, 20, 0);
            lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00FF99), 0); // Mint Green
            lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00FF99), 0);
        }
    }
}

static void ui_update_cb(lv_timer_t* timer) {
    g_timer.update();

    // Update Robot Eyes Animation
    update_robot_eyes();

    // Update 3x Massive Timer Numbers
    uint32_t rem = g_timer.get_remaining_seconds();
    uint32_t mins = rem / 60;
    uint32_t secs = rem % 60;

    char m1[2] = { (char)('0' + (mins / 10)), '\0' };
    char m2[2] = { (char)('0' + (mins % 10)), '\0' };
    char s1[2] = { (char)('0' + (secs / 10)), '\0' };
    char s2[2] = { (char)('0' + (secs % 10)), '\0' };

    lv_label_set_text(label_min1, m1);
    lv_label_set_text(label_min2, m2);
    lv_label_set_text(label_sec1, s1);
    lv_label_set_text(label_sec2, s2);

    // Update Borderless Bottom Message
    lv_label_set_text(label_notice, g_timer.get_message());
}

void ui_timer_init() {
    lv_obj_t* scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0B0D14), 0); // Midnight Dark background
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

    // Left Crying Teardrop
    tear_left = lv_obj_create(top_box);
    lv_obj_set_size(tear_left, 10, 18);
    lv_obj_set_style_bg_color(tear_left, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(tear_left, 5, 0);
    lv_obj_set_style_border_width(tear_left, 0, 0);
    lv_obj_add_flag(tear_left, LV_OBJ_FLAG_HIDDEN);

    // Right Crying Teardrop
    tear_right = lv_obj_create(top_box);
    lv_obj_set_size(tear_right, 10, 18);
    lv_obj_set_style_bg_color(tear_right, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(tear_right, 5, 0);
    lv_obj_set_style_border_width(tear_right, 0, 0);
    lv_obj_add_flag(tear_right, LV_OBJ_FLAG_HIDDEN);

    // ==========================================
    // DIVIDER LINE 1 (Top / Middle)
    // ==========================================
    static lv_point_t line1_points[] = { {20, 125}, {460, 125} };
    lv_obj_t* line1 = lv_line_create(scr);
    lv_line_set_points(line1, line1_points, 2);
    lv_obj_set_style_line_color(line1, lv_color_hex(0x1E2235), 0);
    lv_obj_set_style_line_width(line1, 2, 0);

    // ==========================================
    // 2. MIDDLE SECTION: 3x Massive Timer Numbers (50:00) (~250px)
    // ==========================================
    lv_obj_t* timer_box = lv_obj_create(scr);
    lv_obj_set_size(timer_box, 460, 240);
    lv_obj_align(timer_box, LV_ALIGN_CENTER, 0, -5);
    lv_obj_set_style_bg_opa(timer_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(timer_box, 0, 0);
    lv_obj_clear_flag(timer_box, LV_OBJ_FLAG_SCROLLABLE);

    // Digit 1 (Min tens)
    label_min1 = lv_label_create(timer_box);
    lv_obj_set_style_text_color(label_min1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_min1, &lv_font_montserrat_48, 0);
    lv_obj_set_style_transform_zoom(label_min1, 768, 0); // 3x Bigger Size
    lv_obj_align(label_min1, LV_ALIGN_CENTER, -165, 0);

    // Digit 2 (Min units)
    label_min2 = lv_label_create(timer_box);
    lv_obj_set_style_text_color(label_min2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_min2, &lv_font_montserrat_48, 0);
    lv_obj_set_style_transform_zoom(label_min2, 768, 0); // 3x Bigger Size
    lv_obj_align(label_min2, LV_ALIGN_CENTER, -85, 0);

    // Colon Separator (:)
    label_colon = lv_label_create(timer_box);
    lv_obj_set_style_text_color(label_colon, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_text_font(label_colon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_transform_zoom(label_colon, 768, 0); // 3x Bigger Size
    lv_obj_align(label_colon, LV_ALIGN_CENTER, 0, -10);
    lv_label_set_text(label_colon, ":");

    // Digit 3 (Sec tens)
    label_sec1 = lv_label_create(timer_box);
    lv_obj_set_style_text_color(label_sec1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_sec1, &lv_font_montserrat_48, 0);
    lv_obj_set_style_transform_zoom(label_sec1, 768, 0); // 3x Bigger Size
    lv_obj_align(label_sec1, LV_ALIGN_CENTER, 85, 0);

    // Digit 4 (Sec units)
    label_sec2 = lv_label_create(timer_box);
    lv_obj_set_style_text_color(label_sec2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(label_sec2, &lv_font_montserrat_48, 0);
    lv_obj_set_style_transform_zoom(label_sec2, 768, 0); // 3x Bigger Size
    lv_obj_align(label_sec2, LV_ALIGN_CENTER, 165, 0);

    // ==========================================
    // DIVIDER LINE 2 (Middle / Bottom)
    // ==========================================
    static lv_point_t line2_points[] = { {20, 395}, {460, 395} };
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
    lv_obj_align(label_notice, LV_ALIGN_BOTTOM_MID, 0, -22);
    lv_label_set_text(label_notice, "WELCOME SEUNGPIL!");

    // Refresh UI every 200ms
    ui_refresh_timer = lv_timer_create(ui_update_cb, 200, NULL);
}

void ui_timer_update() {
    // Handled by ui_refresh_timer
}
