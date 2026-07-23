#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Screen Dimensions
#define LCD_WIDTH  480
#define LCD_HEIGHT 480

// Default Timer Duration in Seconds (50 Minutes)
#define DEFAULT_TIMER_SECONDS (50 * 60)

// Official Backlight Pin
#define TFT_BL 38

// ST7701S 3-Wire SPI Pins
#define TFT_CS   39
#define TFT_SCK  48
#define TFT_SDA  47

// RGB Panel Control Pins
#define TFT_DE    18
#define TFT_VSYNC 17
#define TFT_HSYNC 16
#define TFT_PCLK  21

// RGB Data Pins
#define TFT_R0 11
#define TFT_R1 12
#define TFT_R2 13
#define TFT_R3 14
#define TFT_R4 0

#define TFT_G0 8
#define TFT_G1 20
#define TFT_G2 3
#define TFT_G3 46
#define TFT_G4 9
#define TFT_G5 10

#define TFT_B0 4
#define TFT_B1 5
#define TFT_B2 6
#define TFT_B3 7
#define TFT_B4 15

// GT911 Touch I2C Pins (Official: SCL=45, SDA=19)
#define TOUCH_GT911_SCL 45
#define TOUCH_GT911_SDA 19
#define TOUCH_GT911_INT -1
#define TOUCH_GT911_RST -1

// Buzzer Output Pin
#define BUZZER_PIN 1

#endif // CONFIG_H
