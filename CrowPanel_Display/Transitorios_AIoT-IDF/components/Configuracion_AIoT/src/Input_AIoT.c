/* components/Configuracion_AIoT/src/Input_AIoT.c */
#include "esp_lcd_touch_xpt2046.h"
#include "driver/spi_master.h"
#include "lvgl.h"
#include "System_Defines_AIoT.h"
#include "Input_AIoT.h"

static esp_lcd_touch_handle_t tp_handle_AIoT = NULL;
static lv_indev_t *lv_indev_handle = NULL;

// Callback de lectura para LVGL 9.2.0
// Se llama periódicamente para consultar el estado del panel táctil
static void touch_read_cb_AIoT(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t touch_x[1];
    uint16_t touch_y[1];
    uint16_t touch_strength[1];
    uint8_t touch_cnt = 0;

    // Leer datos crudos del controlador
    esp_lcd_touch_read_data(tp_handle_AIoT);
    
    // Obtener coordenadas procesadas (calibradas y promediadas por el driver)
    bool pressed = esp_lcd_touch_get_coordinates(tp_handle_AIoT, touch_x, touch_y, touch_strength, &touch_cnt, 1);

    if (pressed && touch_cnt > 0) {
        data->point.x = touch_x;
        data->point.y = touch_y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void Input_AIoT_Init(void)
{
    // 1. Inicializar Bus SPI (Host SPI2)
    spi_bus_config_t bus_cfg = {
       .mosi_io_num = AIOT_PIN_NUM_TOUCH_MOSI,
       .miso_io_num = AIOT_PIN_NUM_TOUCH_MISO,
       .sclk_io_num = AIOT_PIN_NUM_TOUCH_CLK,
       .quadwp_io_num = -1,
       .quadhd_io_num = -1,
       .max_transfer_sz = 0, // Transferencia máxima por defecto
    };
    // Inicializar bus SPI con DMA habilitado
    ESP_ERROR_CHECK(spi_bus_initialize(AIOT_TOUCH_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    // 2. Configurar Interfaz IO del Panel
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_spi_config_t tp_io_config = ESP_LCD_TOUCH_IO_SPI_XPT2046_CONFIG(AIOT_PIN_NUM_TOUCH_CS);
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)AIOT_TOUCH_SPI_HOST, &tp_io_config, &tp_io_handle));

    // 3. Crear Instancia del Driver Táctil
    esp_lcd_touch_config_t tp_cfg = {
       .x_max = AIOT_LCD_H_RES,
       .y_max = AIOT_LCD_V_RES,
       .rst_gpio_num = -1,
       .int_gpio_num = AIOT_PIN_NUM_TOUCH_IRQ,
       .levels = {
           .reset = 0,
           .interrupt = 0, // XPT2046 interrupción es activa baja
        },
       .flags = {
           .swap_xy = 0,  // Depende de la orientación física del digitalizador
           .mirror_x = 1, // Comúnmente necesario en CrowPanel
           .mirror_y = 1,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_spi_xpt2046(tp_io_handle, &tp_cfg, &tp_handle_AIoT));

    // 4. Registrar Dispositivo de Entrada en LVGL 9.2
    lv_indev_handle = lv_indev_create();
    lv_indev_set_type(lv_indev_handle, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lv_indev_handle, touch_read_cb_AIoT);
}
