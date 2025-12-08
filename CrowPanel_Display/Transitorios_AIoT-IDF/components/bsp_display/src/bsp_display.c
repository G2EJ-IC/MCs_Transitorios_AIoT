#include "bsp_display.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch_xpt2046.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"

static const char *TAG = "BSP_DISPLAY";

// --- PINES CROWPANEL 4.3" ---
#define PIN_BL          GPIO_NUM_2

// 10 MHz: Velocidad balanceada
#define LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000) 

#define LCD_WIDTH          480
#define LCD_HEIGHT         272

// Buffer LVGL: 40 líneas en memoria interna (SRAM) para dibujo rápido
#define LVGL_BUFFER_LINES  40 

// Pines de Datos
#define LCD_PIN_D0          GPIO_NUM_8
#define LCD_PIN_D1          GPIO_NUM_3
#define LCD_PIN_D2          GPIO_NUM_46
#define LCD_PIN_D3          GPIO_NUM_9
#define LCD_PIN_D4          GPIO_NUM_1
#define LCD_PIN_D5          GPIO_NUM_5
#define LCD_PIN_D6          GPIO_NUM_6
#define LCD_PIN_D7          GPIO_NUM_7
#define LCD_PIN_D8          GPIO_NUM_15
#define LCD_PIN_D9          GPIO_NUM_16
#define LCD_PIN_D10         GPIO_NUM_4
#define LCD_PIN_D11         GPIO_NUM_45
#define LCD_PIN_D12         GPIO_NUM_48
#define LCD_PIN_D13         GPIO_NUM_47
#define LCD_PIN_D14         GPIO_NUM_21
#define LCD_PIN_D15         GPIO_NUM_14
#define LCD_PIN_HENABLE     GPIO_NUM_40
#define LCD_PIN_VSYNC       GPIO_NUM_41
#define LCD_PIN_HSYNC       GPIO_NUM_39
#define LCD_PIN_PCLK        GPIO_NUM_42

// Touch
#define TP_SPI_HOST     SPI2_HOST
#define TP_PIN_SCK      GPIO_NUM_12
#define TP_PIN_MOSI     GPIO_NUM_11
#define TP_PIN_MISO     GPIO_NUM_13
#define TP_PIN_CS       GPIO_NUM_0
#define TP_PIN_INT      GPIO_NUM_36

static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;
static lv_display_t *lv_disp = NULL;
static lv_indev_t *lv_indev = NULL;

static void lvgl_tick_task(void *arg) {
    lv_tick_inc(2);
}

// Flush Callback (Partial Mode)
static void bsp_display_lvgl_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    int x1 = area->x1;
    int y1 = area->y1;
    int x2 = area->x2;
    int y2 = area->y2;
    // Mover datos de SRAM (LVGL) a PSRAM (Driver RGB)
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, px_map);
    lv_display_flush_ready(disp);
}

static void bsp_display_lvgl_read(lv_indev_t *indev, lv_indev_data_t *data) {
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)lv_indev_get_user_data(indev);
    uint16_t touch_x[1];
    uint16_t touch_y[1];
    uint8_t touch_cnt = 0;

    esp_lcd_touch_read_data(tp);
    if (esp_lcd_touch_get_coordinates(tp, touch_x, touch_y, NULL, &touch_cnt, 1) && touch_cnt > 0) {
        data->point.x = touch_x[0];
        data->point.y = touch_y[0];
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

esp_err_t bsp_display_init(void)
{
    ESP_LOGI(TAG, "Iniciando Display (Fix Final: Buffer x2)...");

    // 1. Configuración del Panel RGB
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .psram_trans_align = 64,
        .num_fbs = 1,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = GPIO_NUM_NC,
        .pclk_gpio_num = LCD_PIN_PCLK,
        .vsync_gpio_num = LCD_PIN_VSYNC,
        .hsync_gpio_num = LCD_PIN_HSYNC,
        .de_gpio_num = LCD_PIN_HENABLE,
        .data_gpio_nums = {
            LCD_PIN_D0, LCD_PIN_D1, LCD_PIN_D2, LCD_PIN_D3, LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
            LCD_PIN_D8, LCD_PIN_D9, LCD_PIN_D10, LCD_PIN_D11, LCD_PIN_D12, LCD_PIN_D13, LCD_PIN_D14, LCD_PIN_D15,
        },
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ, // 10 MHz
            .h_res = LCD_WIDTH,
            .v_res = LCD_HEIGHT,
            .hsync_back_porch = 43,
            .hsync_front_porch = 8,
            .hsync_pulse_width = 4,
            .vsync_back_porch = 12,
            .vsync_front_porch = 8,
            .vsync_pulse_width = 4,
            .flags.pclk_active_neg = 1, // Mantenemos 1
        },
        .flags.fb_in_psram = 1,
        
        // --- LA SOLUCIÓN MAESTRA ---
        // 2 Líneas de Buffer.
        // 272 / 2 = 136 (Exacto -> No error matemático).
        // Muy pequeño -> Copia ultra rápida -> No error Watchdog.
        .bounce_buffer_size_px = LCD_WIDTH * 2, 
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // 2. Encender Backlight
    gpio_config_t bl_conf = {
        .pin_bit_mask = (1ULL << PIN_BL),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&bl_conf);
    gpio_set_level(PIN_BL, 1);

    // 3. Inicializar Touch (2.5MHz)
    spi_bus_config_t bus_config = {
        .sclk_io_num = TP_PIN_SCK,
        .mosi_io_num = TP_PIN_MOSI,
        .miso_io_num = TP_PIN_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(TP_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_spi_config_t tp_io_config = {
        .cs_gpio_num = TP_PIN_CS,
        .dc_gpio_num = -1,
        .pclk_hz = 2500000, 
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)TP_SPI_HOST, &tp_io_config, &tp_io_handle));

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_WIDTH,
        .y_max = LCD_HEIGHT,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = TP_PIN_INT,
        .levels = { .reset = 0, .interrupt = 0 },
        .flags = { .swap_xy = 0, .mirror_x = 0, .mirror_y = 0 },
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_spi_xpt2046(tp_io_handle, &tp_cfg, &touch_handle));

    // 4. INICIALIZAR LVGL (Modo Parcial)
    lv_init();
    
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvgl_tick_task,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2000));

    lv_disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_color_format(lv_disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_user_data(lv_disp, panel_handle);
    lv_display_set_flush_cb(lv_disp, bsp_display_lvgl_flush);
    
    // Buffers en SRAM para LVGL (Rápido)
    void *buf1 = heap_caps_malloc(LCD_WIDTH * LVGL_BUFFER_LINES * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    void *buf2 = heap_caps_malloc(LCD_WIDTH * LVGL_BUFFER_LINES * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    
    // Partial Mode
    lv_display_set_buffers(lv_disp, buf1, buf2, LCD_WIDTH * LVGL_BUFFER_LINES * sizeof(uint16_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev = lv_indev_create();
    lv_indev_set_type(lv_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_user_data(lv_indev, touch_handle);
    lv_indev_set_read_cb(lv_indev, bsp_display_lvgl_read);

    return ESP_OK;
}

lv_display_t *bsp_get_lv_disp(void) { return lv_disp; }
lv_indev_t *bsp_get_lv_indev(void) { return lv_indev; }
void bsp_display_brightness(uint8_t percent) { gpio_set_level(PIN_BL, 1); }
bool bsp_display_lock(int timeout_ms) { return true; }
void bsp_display_unlock(void) {}
