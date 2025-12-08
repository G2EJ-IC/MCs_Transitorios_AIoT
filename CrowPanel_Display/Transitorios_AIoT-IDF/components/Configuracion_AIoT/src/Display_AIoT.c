/* components/Configuracion_AIoT/src/Display_AIoT.c */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "lvgl.h"
#include "System_Defines_AIoT.h"
#include "Display_AIoT.h"

static const char *TAG = "Display_AIoT";
static esp_lcd_panel_handle_t panel_handle_AIoT = NULL;
static lv_display_t *lv_display_handle = NULL;

// Callback de "Flush" para LVGL 9.2.0
// Esta función es llamada por LVGL cuando un área de la pantalla necesita actualizarse.
// Transfiere el buffer de renderizado de LVGL al driver del panel RGB.
static void display_flush_cb_AIoT(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;

    // Enviar buffer de píxeles al driver del hardware
    // Gracias al Bounce Buffer, esto no bloquea inmediatamente si la PSRAM está ocupada
    esp_lcd_panel_draw_bitmap(panel_handle_AIoT, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);

    // Señalizar a LVGL que la operación de flush está "lista".
    // Nota: En modo RGB con Bounce Buffer, la transferencia es asíncrona pero rápida.
    lv_display_flush_ready(disp);
}

// Función Privada: Inicialización del Hardware RGB
static void init_hardware_rgb_panel(void)
{
    ESP_LOGI(TAG, "Iniciando configuración de Panel RGB AIoT (NV3047)...");

    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,        // RGB565
        .psram_trans_align = 64, // Alineación crítica para DMA en PSRAM
        .num_fbs = 1,            // 1 Framebuffer en PSRAM. Usamos 1 para ahorrar RAM dado el Bounce Buffer.
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = AIOT_PIN_NUM_DISP_EN,
        .pclk_gpio_num = AIOT_PIN_NUM_PCLK,
        .vsync_gpio_num = AIOT_PIN_NUM_VSYNC,
        .hsync_gpio_num = AIOT_PIN_NUM_HSYNC,
        .de_gpio_num = AIOT_PIN_NUM_DE,
        .data_gpio_nums = {
            AIOT_PIN_NUM_DATA0,
            AIOT_PIN_NUM_DATA1,
            AIOT_PIN_NUM_DATA2,
            AIOT_PIN_NUM_DATA3,
            AIOT_PIN_NUM_DATA4,
            AIOT_PIN_NUM_DATA5,
            AIOT_PIN_NUM_DATA6,
            AIOT_PIN_NUM_DATA7,
            AIOT_PIN_NUM_DATA8,
            AIOT_PIN_NUM_DATA9,
            AIOT_PIN_NUM_DATA10,
            AIOT_PIN_NUM_DATA11,
            AIOT_PIN_NUM_DATA12,
            AIOT_PIN_NUM_DATA13,
            AIOT_PIN_NUM_DATA14,
            AIOT_PIN_NUM_DATA15,
        },
        .timings = {
            // Timings ajustados para NV3047 480x272
            // PCLK = 10 MHz
            .pclk_hz = AIOT_LCD_PIXEL_CLOCK_HZ,
            .h_res = AIOT_LCD_H_RES,
            .v_res = AIOT_LCD_V_RES,
            // Porches estándar para estabilidad
            .hsync_back_porch = 43,
            .hsync_front_porch = 8,
            .hsync_pulse_width = 4,
            .vsync_back_porch = 12,
            .vsync_front_porch = 8,
            .vsync_pulse_width = 4,
            .flags.pclk_active_neg = true, // Muestreo en flanco negativo para mayor estabilidad de señal
        },
        .flags.fb_in_psram = 1, // OBLIGATORIO: El framebuffer no cabe en SRAM interna

        // --- CORRECCIÓN DE PARPADEO (FLICKERING) ---
        // Asignamos 10 líneas de buffer en SRAM interna (~10KB).
        // El DMA mueve datos de PSRAM -> SRAM -> LCD.
        .bounce_buffer_size_px = 10 * AIOT_LCD_H_RES,
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle_AIoT));

    // --- CORRECCIÓN DE PANTALLA FRÍA (COLD SCREEN) ---
    // Muchos paneles CrowPanel están cableados como BGR. Forzamos este orden.
    esp_lcd_panel_dev_config_t panel_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR, // CORRECCIÓN CLAVE
        .reset_gpio_num = -1,
    };

    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle_AIoT, &panel_dev_config));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle_AIoT));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle_AIoT, true));

    // Encender Backlight
    gpio_set_direction(AIOT_PIN_NUM_BK_LIGHT, GPIO_MODE_OUTPUT);
    gpio_set_level(AIOT_PIN_NUM_BK_LIGHT, 1);
}

// Inicialización Pública
void Display_AIoT_Init(void)
{
    init_hardware_rgb_panel();

    // Inicialización LVGL 9.2.0
    lv_init();

    // Crear objeto Display en LVGL
    lv_display_handle = lv_display_create(AIOT_LCD_H_RES, AIOT_LCD_V_RES);

    // Asignación de Buffers de Dibujo (Draw Buffers) para LVGL
    // Usamos MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA para asegurar que el buffer de trabajo
    // esté en la SRAM más rápida posible.
    // Buffer parcial: LVGL renderiza aquí, luego Flush callback lo envía al driver.
    void *buf1 = heap_caps_malloc(AIOT_LVGL_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    if (!buf1)
    {
        ESP_LOGE(TAG, "Error crítico: No hay SRAM suficiente para buffer LVGL");
        abort();
    }

    // Configurar buffers en modo PARCIAL
    lv_display_set_buffers(lv_display_handle, buf1, NULL, AIOT_LVGL_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Configurar callback de flush
    lv_display_set_flush_cb(lv_display_handle, display_flush_cb_AIoT);

    // Formato de color: LVGL trabaja internamente. El driver se encarga del BGR.
    lv_display_set_color_format(lv_display_handle, LV_COLOR_FORMAT_RGB565);

    ESP_LOGI(TAG, "Subsistema Display AIoT (LVGL 9.2) Inicializado.");
}
