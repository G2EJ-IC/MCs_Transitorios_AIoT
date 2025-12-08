#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "driver/gpio.h"

// --- PINES CROWPANEL 4.3" ESP32-S3 ---
#define PIN_BL          GPIO_NUM_2

// LCD RGB
#define LCD_d0          GPIO_NUM_8
#define LCD_d1          GPIO_NUM_3
#define LCD_d2          GPIO_NUM_46
#define LCD_d3          GPIO_NUM_9
#define LCD_d4          GPIO_NUM_1
#define LCD_d5          GPIO_NUM_5
#define LCD_d6          GPIO_NUM_6
#define LCD_d7          GPIO_NUM_7
#define LCD_d8          GPIO_NUM_15
#define LCD_d9          GPIO_NUM_16
#define LCD_d10         GPIO_NUM_4
#define LCD_d11         GPIO_NUM_45
#define LCD_d12         GPIO_NUM_48
#define LCD_d13         GPIO_NUM_47
#define LCD_d14         GPIO_NUM_21
#define LCD_d15         GPIO_NUM_14
#define LCD_henable     GPIO_NUM_40
#define LCD_vsync       GPIO_NUM_41
#define LCD_hsync       GPIO_NUM_39
#define LCD_pclk        GPIO_NUM_42

// Touch XPT2046
#define TP_SCK          GPIO_NUM_12
#define TP_MOSI         GPIO_NUM_11
#define TP_MISO         GPIO_NUM_13
#define TP_CS           GPIO_NUM_0
#define TP_INT          GPIO_NUM_36

// Configuración Pantalla
static const uint16_t TFT_WIDTH = 480;
static const uint16_t TFT_HEIGHT = 272;

#endif