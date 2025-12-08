#include "xpt2046_lvgl9.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

static spi_device_handle_t touch_spi_handle;
static int touch_irq_pin;

// Comandos XPT2046 (Start Bit + Channel Sel + Mode + Ref + Power)
#define CMD_X_READ 0x90 // 10010000: CH_X, 12-bit, Differential
#define CMD_Y_READ 0xD0 // 11010000: CH_Y, 12-bit, Differential

// Función auxiliar para transacción SPI
static int16_t spi_transfer_cmd(uint8_t cmd)
{
    uint8_t data_out[1] = {cmd, 0x00, 0x00};
    uint8_t data_in[1] = {0};

    spi_transaction_t t = {
        .length = 24, // 3 bytes: Comando + Byte Alto + Byte Bajo
        .tx_buffer = data_out,
        .rx_buffer = data_in,
        .flags = 0};

    // Usamos polling para baja latencia en interrupciones táctiles
    esp_err_t ret = spi_device_polling_transmit(touch_spi_handle, &t);
    if (ret != ESP_OK)
        return -1;

    // El resultado está en los bits [14:3] de la respuesta de 24 bits recibida
    // data_in es basura/respuesta al comando anterior
    // data_in[2] contiene MSB
    // data_in[3] contiene LSB
    int16_t val = ((data_in[2] << 8) | data_in[3]) >> 3;
    return val;
}

void xpt2046_init_driver(spi_host_device_t host_id, int cs_pin, int irq_pin)
{
    touch_irq_pin = irq_pin;

    // Configurar IRQ como entrada con Pull-Up (Activo Bajo)
    gpio_config_t irq_conf = {
        .pin_bit_mask = (1ULL << irq_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&irq_conf);

    // Configurar Dispositivo SPI
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 2 * 1000 * 1000, // 2 MHz (Seguro para cables largos/ruido)
        .mode = 0,                         // SPI Mode 0
        .spics_io_num = cs_pin,            // CS controlado por hardware
        .queue_size = 1,
        .pre_cb = NULL,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(host_id, &devcfg, &touch_spi_handle));
}

// Callback de lectura compatible con LVGL 9
void xpt2046_read_cb_lvgl9(lv_indev_t *indev, lv_indev_data_t *data)
{
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    // Verificar si la pantalla está siendo tocada (IRQ Low)
    if (gpio_get_level(touch_irq_pin) == 0)
    {

        // --- Algoritmo de Sobremuestreo Simple ---
        int32_t avg_x = 0;
        int32_t avg_y = 0;
        const int samples = 4;

        for (int i = 0; i < samples; i++)
        {
            avg_x += spi_transfer_cmd(CMD_X_READ);
            avg_y += spi_transfer_cmd(CMD_Y_READ);
        }
        intraw_x = avg_x / samples;
        intraw_y = avg_y / samples;

        // --- Calibración y Mapeo ---
        // Valores empíricos típicos para panel de 4.3":
        // X Raw: Min ~300, Max ~3800
        // Y Raw: Min ~250, Max ~3750
        // Nota: A menudo los ejes están invertidos o intercambiados.
        // Asumimos orientación estándar landscape para CrowPanel.

        // Mapeo Lineal: map(value, min_in, max_in, min_out, max_out)
        // Ajuste estos valores 'min/max' observando los logs si el toque está desviado
        int32_t cal_x = (int32_t)((intraw_x - 300) * 480) / (3800 - 300);
        int32_t cal_y = (int32_t)((intraw_y - 250) * 272) / (3750 - 250);

        // Clamping (Restringir a límites de pantalla)
        if (cal_x < 0)
            cal_x = 0;
        if (cal_y < 0)
            cal_y = 0;
        if (cal_x > 479)
            cal_x = 479;
        if (cal_y > 271)
            cal_y = 271;

        data->point.x = cal_x;
        data->point.y = cal_y;
        data->state = LV_INDEV_STATE_PRESSED;

        last_x = cal_x;
        last_y = cal_y;
    }
    else
    {
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
