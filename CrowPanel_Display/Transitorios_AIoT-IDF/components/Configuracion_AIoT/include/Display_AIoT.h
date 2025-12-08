#ifndef DISPLAY_AIOT_H
#define DISPLAY_AIOT_H

#include "esp_err.h"
#include "esp_lcd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Definición de Pines CrowPanel 4.3" Basic (RGB565) ---
#define PIN_NUM_BK_LIGHT       2
#define PIN_NUM_HSYNC          39
#define PIN_NUM_VSYNC          40
#define PIN_NUM_DE             41
#define PIN_NUM_PCLK           42

// Pines de Datos (R0-R4, G0-G5, B0-B4)
#define PIN_NUM_DATA0          8   // B0
#define PIN_NUM_DATA1          3   // B1
#define PIN_NUM_DATA2          46  // B2
#define PIN_NUM_DATA3          9   // B3
#define PIN_NUM_DATA4          1   // B4
#define PIN_NUM_DATA5          5   // G0
#define PIN_NUM_DATA6          6   // G1
#define PIN_NUM_DATA7          7   // G2
#define PIN_NUM_DATA8          15  // G3
#define PIN_NUM_DATA9          16  // G4
#define PIN_NUM_DATA10         4   // G5
#define PIN_NUM_DATA11         45  // R0
#define PIN_NUM_DATA12         48  // R1
#define PIN_NUM_DATA13         47  // R2
#define PIN_NUM_DATA14         21  // R3
#define PIN_NUM_DATA15         14  // R4

// --- Timings NV3047 (480x272) ---
// Frecuencia conservadora para Quad PSRAM (Evita pantalla fría/jitter)
#define LCD_PIXEL_CLOCK_HZ     (9 * 1000 * 1000) 
#define LCD_H_RES              480
#define LCD_V_RES              272

// --- Funciones Públicas ---
/**
 * @brief Inicializa la pantalla CrowPanel Basic corrigiendo el color y frecuencia.
 * @return esp_lcd_panel_handle_t Manejador del panel para LVGL u otros.
 */
esp_lcd_panel_handle_t Init_Display_AIoT(void);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_AIOT_H