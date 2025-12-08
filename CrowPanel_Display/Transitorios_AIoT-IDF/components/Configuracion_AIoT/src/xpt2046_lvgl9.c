#include "xpt2046_lvgl9.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

// Definir variables globales si no estaban
static spi_device_handle_t touch_spi_handle;
static int touch_irq_pin;

// Comandos XPT2046
#define CMD_X_READ 0x90
#define CMD_Y_READ 0xD0

// CORRECCIÓN 1: La función spi_transfer_cmd debe devolver int16_t
static int16_t spi_transfer_cmd(uint8_t cmd)
{
    // CORRECCIÓN 2: data_out debe tener tamaño 3 para enviar [cmd, 0, 0]
    uint8_t data_out[3] = {cmd, 0x00, 0x00};
    uint8_t data_in[3] = {0, 0, 0};

    spi_transaction_t t = {
        .length = 24, // 3 bytes * 8 bits
        .tx_buffer = data_out,
        .rx_buffer = data_in,
        .flags = 0
    };

    esp_err_t ret = spi_device_polling_transmit(touch_spi_handle, &t);
    if (ret != ESP_OK) return -1;

    // Convertir respuesta de 12 bits
    // El XPT2046 devuelve datos en los últimos bits
    int16_t val = ((data_in[1] << 8) | data_in[2]) >> 3;
    return val;
}

// CORRECCIÓN 3: Declarar variables intraw_x / intraw_y
void xpt2046_read_cb_lvgl9(lv_indev_t * indev, lv_indev_data_t * data)
{
    // Variables que faltaban
    int32_t intraw_x = 0;
    int32_t intraw_y = 0;
    int32_t avg_x = 0;
    int32_t avg_y = 0;
    const int samples = 4;

    // Si el pin IRQ está presionado (LOW)
    if (gpio_get_level(touch_irq_pin) == 0) {
        
        for (int i = 0; i < samples; i++) {
            avg_x += spi_transfer_cmd(CMD_X_READ);
            avg_y += spi_transfer_cmd(CMD_Y_READ);
        }
        intraw_x = avg_x / samples;
        intraw_y = avg_y / samples;

        // Calibración CrowPanel 4.3 (Ajustar según necesidad)
        // Ejemplo genérico, puede requerir ajuste fino
        int32_t cal_x = (int32_t)((intraw_x - 300) * 480) / (3800 - 300);
        int32_t cal_y = (int32_t)((intraw_y - 250) * 272) / (3750 - 250);

        // Clamping
        if (cal_x < 0) cal_x = 0;
        if (cal_x > 479) cal_x = 479;
        if (cal_y < 0) cal_y = 0;
        if (cal_y > 271) cal_y = 271;

        data->point.x = cal_x;
        data->point.y = cal_y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void xpt2046_init_driver(spi_host_device_t host, int cs_pin, int irq_pin) {
    touch_irq_pin = irq_pin;
    
    // Configurar IRQ como entrada
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << irq_pin);
    io_conf.pull_up_en = 0; // XPT2046 ya tiene pull-up interno o externo
    gpio_config(&io_conf);

    // Configurar dispositivo SPI
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz es seguro
        .mode = 0,
        .spics_io_num = cs_pin,
        .queue_size = 1,
    };
    
    ESP_ERROR_CHECK(spi_bus_add_device(host, &devcfg, &touch_spi_handle));
}