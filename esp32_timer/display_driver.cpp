#include "display_driver.h"
#include "config.h"
#include <Arduino_GFX_Library.h>

// ST7701S 3-Wire SWSPI Data Bus
static Arduino_DataBus *bus = new Arduino_SWSPI(
    GFX_NOT_DEFINED /* DC */, TFT_CS /* 39 */,
    TFT_SCK /* 48 */, TFT_SDA /* 47 */, GFX_NOT_DEFINED /* MISO */
);

// Proven RGB Panel settings for ESP32-4848S040
static Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    TFT_DE /* 18 */, TFT_VSYNC /* 17 */, TFT_HSYNC /* 16 */, TFT_PCLK /* 21 */,
    TFT_R0 /* 11 */, TFT_R1 /* 12 */, TFT_R2 /* 13 */, TFT_R3 /* 14 */, TFT_R4 /* 0 */,
    TFT_G0 /* 8 */, TFT_G1 /* 20 */, TFT_G2 /* 3 */, TFT_G3 /* 46 */, TFT_G4 /* 9 */, TFT_G5 /* 10 */,
    TFT_B0 /* 4 */, TFT_B1 /* 5 */, TFT_B2 /* 6 */, TFT_B3 /* 7 */, TFT_B4 /* 15 */,
    1 /* hsync_polarity */, 10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
    1 /* vsync_polarity */, 10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */,
    0 /* pclk_active_neg */, 12000000 /* prefer_speed */, false /* useBigEndian */,
    0 /* de_idle_high */, 0 /* pclk_idle_high */, 0 /* bounce_buffer_size_px */
);

// Proven ST7701S Type 9 Init operations (zero flicker & correct colors)
static Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    LCD_WIDTH, LCD_HEIGHT, rgbpanel, 3 /* rotation: 3 is upright */, true /* auto_flush */,
    bus, GFX_NOT_DEFINED /* RST */, st7701_type9_init_operations, sizeof(st7701_type9_init_operations)
);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;

void display_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

    lv_disp_flush_ready(disp_drv);
}

void init_display() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();
    gfx->fillScreen(RGB565_BLACK);

    // Allocate 40-line LVGL draw buffer in SPIRAM/SRAM
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * 40, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!disp_draw_buf) {
        disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LCD_WIDTH * 40, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }

    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, LCD_WIDTH * 40);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.flush_cb = display_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
}
