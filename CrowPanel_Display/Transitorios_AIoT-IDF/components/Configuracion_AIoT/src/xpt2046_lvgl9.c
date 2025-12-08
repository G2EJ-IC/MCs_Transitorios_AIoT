#include "xpt2046_lvgl9.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "XPT2046";

static spi_device_handle_t touch_spi_handle;
static int touch_irq_pin;

// --- CORRECCIÓN DE COMANDOS ---
// Antes estaban al revés. Ahora coinciden con el Datasheet XPT2046.
#define CMD_X_READ  0xD0  // 11010000 (Channel X)
#define CMD_Y_READ  0x90  // 10010000 (Channel Y)

static int16_t spi_transfer_cmd(uint8_t cmd)
{
    uint8_t data_out[3] = {cmd, 0x00, 0x00};
    uint8_t data_in[3] = {0, 0, 0};

    spi_transaction_t t = {
        .length = 24,
        .tx_buffer = data_out,
        .rx_buffer = data_in,
        .flags = 0
    };

    esp_err_t ret = spi_device_polling_transmit(touch_spi_handle, &t);
    if (ret != ESP_OK) return -1;

    int16_t val = ((data_in[1] << 8) | data_in[2]) >> 3;
    return val;
}

void xpt2046_init_driver(spi_host_device_t host, int cs_pin, int irq_pin) 
{
    touch_irq_pin = irq_pin;
    
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << irq_pin);
    io_conf.pull_up_en = 1; 
    gpio_config(&io_conf);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000, 
        .mode = 0,
        .spics_io_num = cs_pin,
        .queue_size = 1,
    };
    
    ESP_ERROR_CHECK(spi_bus_add_device(host, &devcfg, &touch_spi_handle));
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void xpt2046_read_cb_lvgl9(lv_indev_t * indev, lv_indev_data_t * data)
{
    int32_t avg_x = 0;
    int32_t avg_y = 0;
    int32_t intraw_x = 0;
    int32_t intraw_y = 0;
    const int samples = 4;

    if (gpio_get_level(touch_irq_pin) == 0) {
        
        for (int i = 0; i < samples; i++) {
            avg_x += spi_transfer_cmd(CMD_X_READ);
            avg_y += spi_transfer_cmd(CMD_Y_READ);
        }
        intraw_x = avg_x / samples;
        intraw_y = avg_y / samples;

        // --- CALIBRACIÓN NORMAL ---
        // Basada en tu vector: {187, 241, ... 3898, 3769}
        // X: 200 (Izq) -> 3900 (Der)
        // Y: 240 (Arr) -> 3800 (Aba)
        
        int32_t cal_x = map(intraw_x, 200, 3900, 0, 480);
        int32_t cal_y = map(intraw_y, 240, 3800, 0, 272);

        // Clamping
        if (cal_x < 0) cal_x = 0;
        if (cal_x > 479) cal_x = 479;
        if (cal_y < 0) cal_y = 0;
        if (cal_y > 271) cal_y = 271;

        data->point.x = cal_x;
        data->point.y = cal_y;
        data->state = LV_INDEV_STATE_PRESSED;
        
        // Log para verificar coordenadas
        ESP_LOGI(TAG, "X_Raw:%ld -> X:%ld | Y_Raw:%ld -> Y:%ld", intraw_x, (long)cal_x, intraw_y, (long)cal_y);

    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}