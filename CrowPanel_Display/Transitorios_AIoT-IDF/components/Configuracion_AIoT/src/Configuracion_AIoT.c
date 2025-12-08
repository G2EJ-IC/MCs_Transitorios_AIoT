#include "Configuracion_AIoT.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "xpt2046_lvgl9.h" // Driver Táctil Personalizado

static const char *TAG = "HAL_AIoT";

// Manejadores Globales
static esp_lcd_panel_handle_t panel_handle = NULL;
static lv_display_t *display = NULL;
static lv_indev_t *indev = NULL;

// Callback: Notifica a LVGL que el buffer ha sido volcado al hardware
static bool on_trans_done_callback(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    lv_display_flush_ready(display);
    return false;
}

// Callback: Función de dibujo llamada por LVGL
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    // La función esp_lcd maneja internamente la transferencia DMA
    // Nota: x2 y y2 en LVGL son inclusivos, esp_lcd espera exclusivo, por eso el +1
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
}

// Timer para el heartbeat de LVGL
static void lv_tick_task(void *arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

esp_err_t Configuracion_AIoT_Init(void)
{
    ESP_LOGI(TAG, "Iniciando configuración de hardware...");

    // 1. Inicializar LVGL Core
    lv_init();

    // 2. Configurar Panel RGB (esp_lcd)
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,        // RGB565
        .psram_trans_align = 64, // Alineación crítica para DMA en S3
        .num_fbs = 2,            // Doble Buffer para evitar Tearing
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = LCD_PIN_DE,
        .pclk_gpio_num = LCD_PIN_PCLK,
        .vsync_gpio_num = LCD_PIN_VSYNC,
        .hsync_gpio_num = LCD_PIN_HSYNC,
        .data_gpio_nums = {
            LCD_PIN_DATA0, LCD_PIN_DATA1, LCD_PIN_DATA2, LCD_PIN_DATA3,
            LCD_PIN_DATA4, LCD_PIN_DATA5, LCD_PIN_DATA6, LCD_PIN_DATA7,
            LCD_PIN_DATA8, LCD_PIN_DATA9, LCD_PIN_DATA10, LCD_PIN_DATA11,
            LCD_PIN_DATA12, LCD_PIN_DATA13, LCD_PIN_DATA14, LCD_PIN_DATA15},
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES,
            // Timings estándar para pantallas 4.3" 480x272
            .hsync_back_porch = 40,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 8,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
            .flags.pclk_active_neg = 1, // Muestrear en flanco negativo para estabilidad
        },
        .flags.fb_in_psram = 1, // ¡CRÍTICO! Frame buffer en PSRAM
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    // Registrar callbacks de eventos
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_color_trans_done = on_trans_done_callback};
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, NULL));

    // Resetear e iniciar panel
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // 3. Crear Display LVGL 9
    display = lv_display_create(LCD_H_RES, LCD_V_RES);

    // 4. Asignar Buffers de Dibujo
    // Usamos buffers parciales en PSRAM para ahorrar memoria interna para la App
    // Tamaño sugerido: 10% de la pantalla o buffer completo si hay mucha PSRAM
    size_t buf_size = LCD_H_RES * LCD_V_RES * sizeof(lv_color_t); // Buffer completo

    void *buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    void *buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);

    if (buf1 == NULL |

        | buf2 == NULL)
    {
        ESP_LOGE(TAG, "Fallo al asignar memoria en PSRAM para LVGL");
        return ESP_ERR_NO_MEM;
    }

    // Configurar buffers en modo FULL para máximo rendimiento con doble buffer hardware
    lv_display_set_buffers(display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(display, lvgl_flush_cb);

    // 5. Inicializar Touch (XPT2046)
    // Inicializar bus SPI
    spi_bus_config_t buscfg = {
        .mosi_io_num = TOUCH_PIN_MOSI,
        .miso_io_num = TOUCH_PIN_MISO,
        .sclk_io_num = TOUCH_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096};
    // Usamos DMA automático para descargas SPI eficientes
    ESP_ERROR_CHECK(spi_bus_initialize(TOUCH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // Iniciar driver XPT2046 personalizado
    xpt2046_init_driver(TOUCH_SPI_HOST, TOUCH_PIN_CS, TOUCH_PIN_IRQ);

    // Conectar a LVGL
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, xpt2046_read_cb_lvgl9);

    // 6. Timer Periódico
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "lvgl_tick"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LVGL_TICK_PERIOD_MS * 1000));

    // Encender Backlight
    gpio_set_direction(LCD_PIN_BK_LIGHT, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_PIN_BK_LIGHT, 1);

    ESP_LOGI(TAG, "Hardware inicializado correctamente.");
    return ESP_OK;
}
