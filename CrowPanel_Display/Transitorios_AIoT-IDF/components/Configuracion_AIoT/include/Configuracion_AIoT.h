#pragma once

#include "esp_err.h"
#include "lvgl.h"

// =============================================================================
// DEFINICIONES DE HARDWARE - CrowPanel 4.3" Basic (ESP32-S3 RGB Interface)
// =============================================================================

// --- Configuración de Pantalla RGB (NV3047) ---
// Reloj de Píxeles: 10 MHz es un valor seguro para 480x272
#define LCD_PIXEL_CLOCK_HZ     (10 * 1000 * 1000) 

// Pines de Control
#define LCD_PIN_BK_LIGHT       2   // Backlight (PWM capaz)
#define LCD_PIN_HSYNC          39
#define LCD_PIN_VSYNC          40
#define LCD_PIN_DE             41  // Data Enable
#define LCD_PIN_PCLK           0   // GPIO 0 usado como reloj (Cuidado en Boot)

// Pines de Datos (Bus RGB565 de 16 bits)
// Orden de colores: Blue(5) - Green(6) - Red(5)
#define LCD_PIN_DATA0          15  // B0
#define LCD_PIN_DATA1          7   // B1
#define LCD_PIN_DATA2          6   // B2
#define LCD_PIN_DATA3          5   // B3
#define LCD_PIN_DATA4          4   // B4
#define LCD_PIN_DATA5          9   // G0
#define LCD_PIN_DATA6          46  // G1
#define LCD_PIN_DATA7          3   // G2
#define LCD_PIN_DATA8          8   // G3
#define LCD_PIN_DATA9          16  // G4
#define LCD_PIN_DATA10         1   // G5
#define LCD_PIN_DATA11         14  // R0
#define LCD_PIN_DATA12         21  // R1
#define LCD_PIN_DATA13         47  // R2
#define LCD_PIN_DATA14         48  // R3
#define LCD_PIN_DATA15         45  // R4

// Resolución
#define LCD_H_RES              480
#define LCD_V_RES              272

// --- Configuración Táctil (XPT2046) ---
// Se utiliza el host SPI2 (VSPI) para separar del bus Flash interno
#define TOUCH_SPI_HOST         SPI2_HOST 
#define TOUCH_PIN_MISO         13
#define TOUCH_PIN_MOSI         11
#define TOUCH_PIN_CLK          12
#define TOUCH_PIN_CS           38  // Chip Select (Corregido para evitar conflicto PCLK)
#define TOUCH_PIN_IRQ          36  // Interrupción (Activo en bajo)

// --- Configuración LVGL ---
#define LVGL_TICK_PERIOD_MS    2

/**
 * @brief Inicializa todo el hardware AIoT (LCD, Touch, LVGL).
 * Configura los buses, asigna memoria PSRAM para buffers e inicia los timers.
 * @return ESP_OK si la inicialización es exitosa.
 */
esp_err_t Configuracion_AIoT_Init(void);
