#include "ui_timer.h"
#include "timer_logic.h"
#include "config.h"

// ============================================================================
// Akno Robot 15 Facial Expression Engine Implementation
// ============================================================================

enum AknoExpr {
    EXPR_NORMAL = 0,
    EXPR_HAPPY,
    EXPR_SAD,
    EXPR_ANGRY,
    EXPR_UPSET,
    EXPR_CUTE,
    EXPR_SUSPICIOUS,
    EXPR_IAM,
    EXPR_YOUMADEMEE,
    EXPR_WONDER,
    EXPR_DOWN,
    EXPR_RIGHT,
    EXPR_LEFT,
    EXPR_UP,
    EXPR_BLINK
};

static AknoExpr current_expr = EXPR_NORMAL;

static lv_obj_t* eye_left;
static lv_obj_t* eye_right;
static lv_obj_t* eyelid_top_left;
static lv_obj_t* eyelid_top_right;
static lv_obj_t* eyelid_bottom_left;
static lv_obj_t* eyelid_bottom_right;
static lv_obj_t* sweat_drop;

// Digit objects for 120px Vector Bold Digital Timer Display
static lv_obj_t* digit_segs[4][7];
static lv_obj_t* colon_dot1;
static lv_obj_t* colon_dot2;

static lv_obj_t* label_notice;
static lv_timer_t* ui_refresh_timer;

static uint32_t anim_frame = 0;

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

static void create_digital_digit(lv_obj_t* parent, int idx, int x_center) {
    int w = 62;
    int h = 130;
    int t = 12; // Segment thickness 12px
    int half_h = h / 2;

    digit_segs[idx][0] = lv_obj_create(parent); // Seg A
    lv_obj_set_size(digit_segs[idx][0], w - 4, t);
    lv_obj_align(digit_segs[idx][0], LV_ALIGN_CENTER, x_center, -half_h);

    digit_segs[idx][1] = lv_obj_create(parent); // Seg B
    lv_obj_set_size(digit_segs[idx][1], t, half_h - 2);
    lv_obj_align(digit_segs[idx][1], LV_ALIGN_CENTER, x_center + (w/2) - (t/2), -half_h/2);

    digit_segs[idx][2] = lv_obj_create(parent); // Seg C
    lv_obj_set_size(digit_segs[idx][2], t, half_h - 2);
    lv_obj_align(digit_segs[idx][2], LV_ALIGN_CENTER, x_center + (w/2) - (t/2), half_h/2);

    digit_segs[idx][3] = lv_obj_create(parent); // Seg D
    lv_obj_set_size(digit_segs[idx][3], w - 4, t);
    lv_obj_align(digit_segs[idx][3], LV_ALIGN_CENTER, x_center, half_h);

    digit_segs[idx][4] = lv_obj_create(parent); // Seg E
    lv_obj_set_size(digit_segs[idx][4], t, half_h - 2);
    lv_obj_align(digit_segs[idx][4], LV_ALIGN_CENTER, x_center - (w/2) + (t/2), half_h/2);

    digit_segs[idx][5] = lv_obj_create(parent); // Seg F
    lv_obj_set_size(digit_segs[idx][5], t, half_h - 2);
    lv_obj_align(digit_segs[idx][5], LV_ALIGN_CENTER, x_center - (w/2) + (t/2), -half_h/2);

    digit_segs[idx][6] = lv_obj_create(parent); // Seg G
    lv_obj_set_size(digit_segs[idx][6], w - 4, t);
    lv_obj_align(digit_segs[idx][6], LV_ALIGN_CENTER, x_center, 0);

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
            lv_obj_set_style_bg_color(digit_segs[idx][s], lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_bg_opa(digit_segs[idx][s], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_bg_color(digit_segs[idx][s], lv_color_hex(0x161A26), 0);
            lv_obj_set_style_bg_opa(digit_segs[idx][s], LV_OPA_COVER, 0);
        }
    }
}

// Set Akno Expression
void ui_set_expression(int expr_id) {
    if (expr_id >= 0 && expr_id <= 14) {
        current_expr = (AknoExpr)expr_id;
    }
}

// Akno Robot 15 Facial Expressions Renderer
static void update_akno_expressions() {
    anim_frame++;
    TimerState st = g_timer.get_state();

    // Default Eye Layout
    int left_x = -50;
    int right_x = 50;
    int eye_y = 0;
    int eye_w = 48;
    int eye_h = 48;
    int radius = 24;

    uint32_t color_left = 0x00E5FF;  // Neon Cyan
    uint32_t color_right = 0x00E5FF;

    lv_obj_set_size(eyelid_top_left, 0, 0);
    lv_obj_set_size(eyelid_top_right, 0, 0);
    lv_obj_set_style_bg_opa(eyelid_bottom_left, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(eyelid_bottom_right, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(sweat_drop, LV_OBJ_FLAG_HIDDEN);

    // Auto State Override if timer running/paused/finished
    AknoExpr expr = current_expr;
    if (st == TIMER_RUNNING && current_expr == EXPR_NORMAL) expr = EXPR_ANGRY;
    else if (st == TIMER_PAUSED && current_expr == EXPR_NORMAL) expr = EXPR_SAD;
    else if (st == TIMER_FINISHED && current_expr == EXPR_NORMAL) expr = EXPR_HAPPY;

    switch (expr) {
        case EXPR_HAPPY: // 1. Happy (^ ^)
            eye_w = 54; eye_h = 54; radius = 27;
            color_left = 0xFFD700; color_right = 0xFFD700; // Gold
            lv_obj_set_style_bg_opa(eyelid_bottom_left, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_opa(eyelid_bottom_right, LV_OPA_COVER, 0);
            if (anim_frame % 2 == 0) eye_y -= 3;
            break;

        case EXPR_SAD: // 2. Sad / Crying (T T)
            eye_w = 44; eye_h = 16; radius = 8;
            color_left = 0xFF3366; color_right = 0xFF3366;
            lv_obj_clear_flag(sweat_drop, LV_OBJ_FLAG_HIDDEN);
            lv_obj_align(sweat_drop, LV_ALIGN_CENTER, 85, -10 + ((anim_frame * 3) % 40));
            break;

        case EXPR_ANGRY: // 3. Angry (> <)
            eye_w = 54; eye_h = 42; radius = 16;
            color_left = 0xFF3300; color_right = 0xFF3300;
            lv_obj_set_size(eyelid_top_left, 56, 18);
            lv_obj_set_size(eyelid_top_right, 56, 18);
            lv_obj_align_to(eyelid_top_left, eye_left, LV_ALIGN_TOP_MID, 0, -4);
            lv_obj_align_to(eyelid_top_right, eye_right, LV_ALIGN_TOP_MID, 0, -4);
            break;

        case EXPR_UPSET: // 4. Upset (Pouting)
            eye_w = 40; eye_h = 30; radius = 12;
            color_left = 0xFF9900; color_right = 0xFF9900;
            left_x = -40; right_x = 40;
            break;

        case EXPR_CUTE: // 5. Cute (Sparkling Big Eyes)
            eye_w = 58; eye_h = 58; radius = 29;
            color_left = 0xFF00FF; color_right = 0xFF00FF; // Magenta
            if ((anim_frame / 4) % 2 == 0) { eye_w = 60; eye_h = 60; }
            break;

        case EXPR_SUSPICIOUS: // 6. Suspicious (Side-eye Glance)
            eye_w = 42; eye_h = 24; radius = 10;
            left_x = -70; right_x = 30; // Looking far left
            color_left = 0x00E5FF; color_right = 0x00E5FF;
            break;

        case EXPR_IAM: // 7. I am / Proud Arrogant Look
            eye_w = 52; eye_h = 14; radius = 6;
            color_left = 0x00FF99; color_right = 0x00FF99;
            eye_y = -8;
            break;

        case EXPR_YOUMADEMEE: // 8. You Made Me (Raging Red Eyes)
            eye_w = 60; eye_h = 36; radius = 10;
            color_left = 0xFF0000; color_right = 0xFF0000;
            if (anim_frame % 2 == 0) left_x += 2; else left_x -= 2;
            break;

        case EXPR_WONDER: // 9. Wonder / Curious (Uneven Eyes)
            eye_w = 54; eye_h = 54;
            lv_obj_set_size(eye_left, 54, 54);
            lv_obj_set_size(eye_right, 36, 36);
            color_left = 0x00E5FF; color_right = 0x00E5FF;
            eye_y = -6;
            break;

        case EXPR_DOWN: // 10. Look Down
            eye_y = 12;
            break;

        case EXPR_RIGHT: // 11. Look Right
            left_x = -35; right_x = 65;
            break;

        case EXPR_LEFT: // 12. Look Left
            left_x = -65; right_x = 35;
            break;

        case EXPR_UP: // 13. Look Up
            eye_y = -12;
            break;

        case EXPR_BLINK: // 14. Blink
            eye_h = 6; radius = 3;
            break;

        case EXPR_NORMAL: // 0. Normal Idle Loop (Blink & Gaze cycle)
        default:
            if (anim_frame % 15 == 0) {
                eye_h = 6; radius = 3;
            } else if ((anim_frame / 20) % 4 == 1) {
                left_x = -65; right_x = 35; // Look Left
            } else if ((anim_frame / 20) % 4 == 3) {
                left_x = -35; right_x = 65; // Look Right
            }
            break;
    }

    lv_obj_set_size(eye_left, eye_w, eye_h);
    lv_obj_set_size(eye_right, eye_w, eye_h);
    lv_obj_set_style_radius(eye_left, radius, 0);
    lv_obj_set_style_radius(eye_right, radius, 0);
    lv_obj_set_style_bg_color(eye_left, lv_color_hex(color_left), 0);
    lv_obj_set_style_bg_color(eye_right, lv_color_hex(color_right), 0);

    lv_obj_align(eye_left, LV_ALIGN_CENTER, left_x, eye_y);
    lv_obj_align(eye_right, LV_ALIGN_CENTER, right_x, eye_y);
}

static void ui_update_cb(lv_timer_t* timer) {
    g_timer.update();

    update_akno_expressions();

    // Update 120px Bold Digital Segments
    uint32_t rem = g_timer.get_remaining_seconds();
    uint32_t mins = rem / 60;
    uint32_t secs = rem % 60;

    update_digit_value(0, mins / 10);
    update_digit_value(1, mins % 10);
    update_digit_value(2, secs / 10);
    update_digit_value(3, secs % 10);

    // Blink Colon Dots
    if ((anim_frame / 3) % 2 == 0) {
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
    // 1. TOP SECTION: Akno 15 Expression Robot Eyes Container (~120px)
    // ==========================================
    lv_obj_t* top_box = lv_obj_create(scr);
    lv_obj_set_size(top_box, 440, 110);
    lv_obj_align(top_box, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_bg_opa(top_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(top_box, 0, 0);
    lv_obj_clear_flag(top_box, LV_OBJ_FLAG_SCROLLABLE);

    eye_left = lv_obj_create(top_box);
    lv_obj_set_size(eye_left, 48, 48);
    lv_obj_set_style_bg_color(eye_left, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(eye_left, 24, 0);
    lv_obj_set_style_border_width(eye_left, 0, 0);
    lv_obj_clear_flag(eye_left, LV_OBJ_FLAG_SCROLLABLE);

    eye_right = lv_obj_create(top_box);
    lv_obj_set_size(eye_right, 48, 48);
    lv_obj_set_style_bg_color(eye_right, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(eye_right, 24, 0);
    lv_obj_set_style_border_width(eye_right, 0, 0);
    lv_obj_clear_flag(eye_right, LV_OBJ_FLAG_SCROLLABLE);

    eyelid_top_left = lv_obj_create(top_box);
    lv_obj_set_style_bg_color(eyelid_top_left, lv_color_hex(0x0B0D14), 0);
    lv_obj_set_style_border_width(eyelid_top_left, 0, 0);
    lv_obj_clear_flag(eyelid_top_left, LV_OBJ_FLAG_SCROLLABLE);

    eyelid_top_right = lv_obj_create(top_box);
    lv_obj_set_style_bg_color(eyelid_top_right, lv_color_hex(0x0B0D14), 0);
    lv_obj_set_style_border_width(eyelid_top_right, 0, 0);
    lv_obj_clear_flag(eyelid_top_right, LV_OBJ_FLAG_SCROLLABLE);

    eyelid_bottom_left = lv_obj_create(top_box);
    lv_obj_set_size(eyelid_bottom_left, 44, 20);
    lv_obj_align_to(eyelid_bottom_left, eye_left, LV_ALIGN_BOTTOM_MID, 0, 4);
    lv_obj_set_style_bg_color(eyelid_bottom_left, lv_color_hex(0x0B0D14), 0);
    lv_obj_set_style_radius(eyelid_bottom_left, 10, 0);
    lv_obj_set_style_border_width(eyelid_bottom_left, 0, 0);

    eyelid_bottom_right = lv_obj_create(top_box);
    lv_obj_set_size(eyelid_bottom_right, 44, 20);
    lv_obj_align_to(eyelid_bottom_right, eye_right, LV_ALIGN_BOTTOM_MID, 0, 4);
    lv_obj_set_style_bg_color(eyelid_bottom_right, lv_color_hex(0x0B0D14), 0);
    lv_obj_set_style_radius(eyelid_bottom_right, 10, 0);
    lv_obj_set_style_border_width(eyelid_bottom_right, 0, 0);

    sweat_drop = lv_obj_create(top_box);
    lv_obj_set_size(sweat_drop, 10, 16);
    lv_obj_set_style_bg_color(sweat_drop, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(sweat_drop, 5, 0);
    lv_obj_set_style_border_width(sweat_drop, 0, 0);
    lv_obj_add_flag(sweat_drop, LV_OBJ_FLAG_HIDDEN);

    // DIVIDER LINE 1
    static lv_point_t line1_points[] = { {20, 125}, {460, 125} };
    lv_obj_t* line1 = lv_line_create(scr);
    lv_line_set_points(line1, line1_points, 2);
    lv_obj_set_style_line_color(line1, lv_color_hex(0x1E2235), 0);
    lv_obj_set_style_line_width(line1, 2, 0);

    // ==========================================
    // 2. MIDDLE SECTION: 120px Vector Bold Digital Digits (~250px)
    // ==========================================
    lv_obj_t* timer_box = lv_obj_create(scr);
    lv_obj_set_size(timer_box, 460, 250);
    lv_obj_align(timer_box, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(timer_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(timer_box, 0, 0);
    lv_obj_clear_flag(timer_box, LV_OBJ_FLAG_SCROLLABLE);

    create_digital_digit(timer_box, 0, -150);
    create_digital_digit(timer_box, 1, -60);
    create_digital_digit(timer_box, 2, 60);
    create_digital_digit(timer_box, 3, 150);

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
