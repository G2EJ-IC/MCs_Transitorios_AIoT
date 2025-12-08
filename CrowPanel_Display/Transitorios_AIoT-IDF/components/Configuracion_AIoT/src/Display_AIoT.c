#include "Display_AIoT.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "Display_AIoT";

esp_lcd_panel_handle_t Init_Display_AIoT(void)
{
    ESP_LOGI(TAG, "Iniciando Driver AIoT para CrowPanel Basic (NV3047)...");

    esp_lcd_panel_handle_t panel_handle = NULL;

    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565
        .psram_trans_align = 64,
        .num_fbs = 1,     // 1 Framebuffer en PSRAM
        
        // --- CORRECCIÓN CRÍTICA PARA BASIC (Quad PSRAM) ---
        // Buffer intermedio en SRAM interna. Vital para estabilidad a 9MHz.
        .bounce_buffer_size_px = 480 * 10, 
        
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = -1,
        .pclk_gpio_num = PIN_NUM_PCLK,
        .vsync_gpio_num = PIN_NUM_VSYNC,
        .hsync_gpio_num = PIN_NUM_HSYNC,
        .de_gpio_num = PIN_NUM_DE,
        .data_gpio_nums = {
            PIN_NUM_DATA0, PIN_NUM_DATA1, PIN_NUM_DATA2, PIN_NUM_DATA3, PIN_NUM_DATA4,
            PIN_NUM_DATA5, PIN_NUM_DATA6, PIN_NUM_DATA7, PIN_NUM_DATA8, PIN_NUM_DATA9,
            PIN_NUM_DATA10, PIN_NUM_DATA11, PIN_NUM_DATA12, PIN_NUM_DATA13, PIN_NUM_DATA14,
            PIN_NUM_DATA15,
        },
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES,
            // Porches ajustados para centrar la imagen
            .hsync_back_porch = 43,
            .hsync_front_porch = 8,
            .hsync_pulse_width = 4,
            .vsync_back_porch = 12,
            .vsync_front_porch = 8,
            .vsync_pulse_width = 4,
            // Corrección de color y estabilidad de fase
            .flags.pclk_active_neg = true, 
        },
        .flags.fb_in_psram = true,
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Encender Backlight
    gpio_set_direction((gpio_num_t)PIN_NUM_BK_LIGHT, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)PIN_NUM_BK_LIGHT, 1);

    ESP_LOGI(TAG, "Pantalla AIoT Inicializada Correctamente.");
    return panel_handle;
}