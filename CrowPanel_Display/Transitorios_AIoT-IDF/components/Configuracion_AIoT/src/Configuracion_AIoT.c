#include "Configuracion_AIoT.h"
#include "System_Defines_AIoT.h"
#include "xpt2046_lvgl9.h" // Asegúrate de tener este header o definir las funciones aquí

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"

static const char *TAG = "Config_AIoT";

// --- Variables Globales del Driver ---
static esp_lcd_panel_handle_t panel_handle = NULL;
static lv_display_t *lv_disp = NULL;
static lv_indev_t *lv_indev = NULL;

// --- Funciones Internas ---

static void lvgl_tick_task(void *arg) {
    lv_tick_inc(2); // Tick de 2ms
}

// Flush Callback: Copia de SRAM (LVGL) a PSRAM (Driver RGB)
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2;
    int y2 = area->y2;
    
    // Función oficial de copiado con gestión automática de caché
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, px_map);
    lv_display_flush_ready(disp);
}

// --- Inicialización Principal ---
esp_err_t Configuracion_AIoT_Init(void) {
    ESP_LOGI(TAG, "Iniciando Hardware CrowPanel 4.3...");

    // 1. Configurar Panel RGB
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565
        .psram_trans_align = 64,
        .num_fbs = 1,     // 1 Buffer de cuadro completo en PSRAM
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = AIOT_PIN_NUM_DISP_EN,
        .pclk_gpio_num = AIOT_PIN_NUM_PCLK,
        .vsync_gpio_num = AIOT_PIN_NUM_VSYNC,
        .hsync_gpio_num = AIOT_PIN_NUM_HSYNC,
        .de_gpio_num = AIOT_PIN_NUM_DE,
        .data_gpio_nums = {
            AIOT_PIN_NUM_DATA0, AIOT_PIN_NUM_DATA1, AIOT_PIN_NUM_DATA2, AIOT_PIN_NUM_DATA3,
            AIOT_PIN_NUM_DATA4, AIOT_PIN_NUM_DATA5, AIOT_PIN_NUM_DATA6, AIOT_PIN_NUM_DATA7,
            AIOT_PIN_NUM_DATA8, AIOT_PIN_NUM_DATA9, AIOT_PIN_NUM_DATA10, AIOT_PIN_NUM_DATA11,
            AIOT_PIN_NUM_DATA12, AIOT_PIN_NUM_DATA13, AIOT_PIN_NUM_DATA14, AIOT_PIN_NUM_DATA15,
        },
        .timings = {
            .pclk_hz = AIOT_LCD_PIXEL_CLOCK_HZ,
            .h_res = AIOT_LCD_H_RES,
            .v_res = AIOT_LCD_V_RES,
            .hsync_back_porch = 43, .hsync_front_porch = 8, .hsync_pulse_width = 4,
            .vsync_back_porch = 12, .vsync_front_porch = 8, .vsync_pulse_width = 4,
            .flags.pclk_active_neg = 1, // Importante para estabilidad
        },
        .flags.fb_in_psram = 1, // Framebuffer principal en PSRAM
        .bounce_buffer_size_px = AIOT_LCD_H_RES * 10, // Buffer DMA interno para estabilidad
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // 2. Backlight (Encendido directo)
    gpio_config_t bl_conf = {
        .pin_bit_mask = (1ULL << AIOT_PIN_NUM_BK_LIGHT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0, .pull_down_en = 0, .intr_type = 0
    };
    gpio_config(&bl_conf);
    gpio_set_level(AIOT_PIN_NUM_BK_LIGHT, 1);

    // 3. Inicializar LVGL
    lv_init();
    
    // Timer para LVGL tick
    const esp_timer_create_args_t lvgl_tick_timer_args = { .callback = &lvgl_tick_task, .name = "lvgl_tick" };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2000));

    // Crear Display LVGL
    lv_disp = lv_display_create(AIOT_LCD_H_RES, AIOT_LCD_V_RES);
    lv_display_set_user_data(lv_disp, panel_handle);
    lv_display_set_flush_cb(lv_disp, lvgl_flush_cb);

    // --- ESTRATEGIA DE MEMORIA: MODO PARCIAL (ROBUSTO) ---
    // Asignamos buffers pequeños en SRAM (Interna) para máxima velocidad de dibujo.
    // 20 líneas x 480 px x 2 bytes = ~19KB. Cabe perfecto en interna.
    #define BUFFER_LINES 20
    void *buf1 = heap_caps_malloc(AIOT_LCD_H_RES * BUFFER_LINES * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    void *buf2 = heap_caps_malloc(AIOT_LCD_H_RES * BUFFER_LINES * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    
    lv_display_set_buffers(lv_disp, buf1, buf2, AIOT_LCD_H_RES * BUFFER_LINES * sizeof(uint16_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // 4. Inicializar Touch (Llamada al driver XPT2046)
    // Asegúrate de que las funciones de xpt2046_lvgl9.c estén disponibles
    // xpt2046_init(); // Si tienes una función de init
    // lv_indev = lv_indev_create();
    // ... configuración del indev ...

    return ESP_OK;
}