#include "ui_timer.h"
#include "timer_logic.h"
#include "config.h"
#include <AnimatedGIF.h>
#include "gif_data.h"

// ============================================================================
// State-Based GIF Expression Engine for ESP32-S3
// Default (Idle/Running), Surprise (Paused), Crying (Finished)
// ============================================================================

enum GifMode {
    MODE_NONE = -1,
    MODE_DEFAULT = 0,
    MODE_SURPRISE,
    MODE_CRYING
};

static AnimatedGIF gif;
static GifMode active_gif_mode = MODE_NONE;
static bool gif_playing = false;

static lv_obj_t* canvas_gif;
static lv_color_t canvas_buf[240 * 100];

// 4 Digit objects for 120px Vector Bold Digital Timer Display
static lv_obj_t* digit_segs[4][7];
static lv_obj_t* colon_dot1;
static lv_obj_t* colon_dot2;

static lv_obj_t* label_notice;
static lv_timer_t* ui_refresh_timer;

static uint32_t anim_tick = 0;

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

// GIF Pixel Line Renderer Callback
static void GIFDraw(GIFDRAW *pDraw) {
    uint8_t *s;
    uint16_t *usPalette;
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > 240)
        iWidth = 240 - pDraw->iX;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y;

    if (y >= 100) return;

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) {
        for (x = 0; x < iWidth; x++) {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    for (x = 0; x < iWidth; x++) {
        uint8_t c = s[x];
        if (!pDraw->ucHasTransparency || c != pDraw->ucTransparent) {
            uint16_t rgb565 = usPalette[c];
            uint8_t r = ((rgb565 >> 11) & 0x1F) << 3;
            uint8_t g = ((rgb565 >> 5) & 0x3F) << 2;
            uint8_t b = (rgb565 & 0x1F) << 3;
            int px = pDraw->iX + x;
            if (px < 240) {
                canvas_buf[y * 240 + px] = lv_color_make(r, g, b);
            }
        }
    }
}

static void set_gif_mode(GifMode mode) {
    if (active_gif_mode == mode && gif_playing) return;

    active_gif_mode = mode;
    if (gif_playing) {
        gif.close();
        gif_playing = false;
    }

    const uint8_t* pData = gif_data_default;
    size_t sz = GIF_DEFAULT_SIZE;

    if (mode == MODE_SURPRISE) {
        pData = gif_data_surprise;
        sz = GIF_SURPRISE_SIZE;
    } else if (mode == MODE_CRYING) {
        pData = gif_data_crying;
        sz = GIF_CRYING_SIZE;
    }

    gif.begin(GIF_PALETTE_RGB565_LE);
    if (gif.open((uint8_t*)pData, sz, GIFDraw)) {
        gif_playing = true;
    }
}

static void update_gif_player() {
    TimerState st = g_timer.get_state();

    if (st == TIMER_PAUSED) {
        set_gif_mode(MODE_SURPRISE);
    } else if (st == TIMER_FINISHED) {
        set_gif_mode(MODE_CRYING);
    } else {
        set_gif_mode(MODE_DEFAULT);
    }

    if (gif_playing) {
        int res = gif.playFrame(false, NULL);
        if (res == 0) {
            gif.reset(); // Continuous loop for current state GIF
        }
    }
    lv_obj_invalidate(canvas_gif);
}

static void create_digital_digit(lv_obj_t* parent, int idx, int x_center) {
    int w = 62;
    int h = 130;
    int t = 12;
    int half_h = h / 2;

    digit_segs[idx][0] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][0], w - 4, t);
    lv_obj_align(digit_segs[idx][0], LV_ALIGN_CENTER, x_center, -half_h);

    digit_segs[idx][1] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][1], t, half_h - 2);
    lv_obj_align(digit_segs[idx][1], LV_ALIGN_CENTER, x_center + (w/2) - (t/2), -half_h/2);

    digit_segs[idx][2] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][2], t, half_h - 2);
    lv_obj_align(digit_segs[idx][2], LV_ALIGN_CENTER, x_center + (w/2) - (t/2), half_h/2);

    digit_segs[idx][3] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][3], w - 4, t);
    lv_obj_align(digit_segs[idx][3], LV_ALIGN_CENTER, x_center, half_h);

    digit_segs[idx][4] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][4], t, half_h - 2);
    lv_obj_align(digit_segs[idx][4], LV_ALIGN_CENTER, x_center - (w/2) + (t/2), half_h/2);

    digit_segs[idx][5] = lv_obj_create(parent);
    lv_obj_set_size(digit_segs[idx][5], t, half_h - 2);
    lv_obj_align(digit_segs[idx][5], LV_ALIGN_CENTER, x_center - (w/2) + (t/2), -half_h/2);

    digit_segs[idx][6] = lv_obj_create(parent);
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

void ui_set_expression(int expr_id) {
    if (expr_id == 1) set_gif_mode(MODE_SURPRISE);
    else if (expr_id == 2) set_gif_mode(MODE_CRYING);
    else set_gif_mode(MODE_DEFAULT);
}

static void ui_update_cb(lv_timer_t* timer) {
    anim_tick++;
    g_timer.update();

    // Play State-Based GIF Animation
    update_gif_player();

    // Update 120px Bold Digital Segments
    uint32_t rem = g_timer.get_remaining_seconds();
    uint32_t mins = rem / 60;
    uint32_t secs = rem % 60;

    update_digit_value(0, mins / 10);
    update_digit_value(1, mins % 10);
    update_digit_value(2, secs / 10);
    update_digit_value(3, secs % 10);

    // Blink Colon Dots
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
    // 1. TOP SECTION: State-Based GIF Canvas (~120px)
    // ==========================================
    canvas_gif = lv_canvas_create(scr);
    lv_canvas_set_buffer(canvas_gif, canvas_buf, 240, 100, LV_IMG_CF_TRUE_COLOR);
    lv_canvas_fill_bg(canvas_gif, lv_color_hex(0x0B0D14), LV_OPA_COVER);
    lv_obj_align(canvas_gif, LV_ALIGN_TOP_MID, 0, 15);

    set_gif_mode(MODE_DEFAULT);

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

    ui_refresh_timer = lv_timer_create(ui_update_cb, 80, NULL);
}

void ui_timer_update() {
    // Handled by ui_refresh_timer
}
