#include "System_Control_AIoT.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "lvgl.h" 
#include <stdio.h>

static const char *TAG = "System_Control";

#define BL_PIN 2

// Variables internas
static uint32_t time_limit_ms = 15000; 
static bool estado_actual_on = true;   

void sys_control_aiot_init(void)
{
    // Configuración del Timer PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_10_BIT, 
        .freq_hz = 5000,                      
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configuración inicial del Canal
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = BL_PIN,
        .duty = 1023, 
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

// --- FUNCIÓN CORREGIDA ---
void sys_control_aiot_set_backlight(int32_t percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    if (percent == 0) {
        // APAGAR: Robamos el pin para GPIO y forzamos Tierra (GND)
        ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0); 
        gpio_set_direction(BL_PIN, GPIO_MODE_OUTPUT);
        gpio_set_level(BL_PIN, 0); 
    } else {
        // ENCENDER: ¡DEVOLVEMOS EL PIN AL PWM!
        // Esto equivale a tu 'ledcAttachPin' de PlatformIO
        ledc_channel_config_t ledc_channel = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .timer_sel = LEDC_TIMER_0,
            .intr_type = LEDC_INTR_DISABLE,
            .gpio_num = BL_PIN, // Aquí se re-conecta el pin al PWM
            .duty = (percent * 1023) / 100, 
            .hpoint = 0
        };
        ledc_channel_config(&ledc_channel); // Aplicamos la config para reconectar
        
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
}

void sys_control_aiot_get_uptime(char *buffer, size_t size)
{
    int64_t time_us = esp_timer_get_time();
    uint64_t total_ms = time_us / 1000;
    int seg_t = total_ms / 1000;
    int dia = seg_t / 86400; seg_t %= 86400;
    int hora = seg_t / 3600; seg_t %= 3600;
    int min = seg_t / 60; seg_t %= 60;
    int seg = seg_t;
    snprintf(buffer, size, "%02d d %02d h %02d m %02d s", dia, hora, min, seg);
}

void sys_control_aiot_set_sleep_index(int index)
{
    switch (index) {
        case 0: time_limit_ms = 15000; break;      
        case 1: time_limit_ms = 30000; break;      
        case 2: time_limit_ms = 60000; break;      
        case 3: time_limit_ms = 120000; break;     
        case 4: time_limit_ms = 300000; break;     
        case 5: time_limit_ms = 600000; break;     
        default: time_limit_ms = 15000; break;
    }
    lv_disp_trig_activity(NULL);
    if (!estado_actual_on) {
        sys_control_aiot_set_backlight(100);
        estado_actual_on = true;
    }
}

void sys_control_aiot_manage_sleep(void)
{
    static int64_t momento_apagado = 0;
    int64_t ahora = esp_timer_get_time() / 1000;
    
    uint32_t inactividad = lv_disp_get_inactive_time(NULL);

    if (inactividad < time_limit_ms) 
    {
        // INTENTO DE ENCENDIDO
        if (estado_actual_on == false) {
            // Filtro anti-rebote (1.5 segundos de seguridad)
            if ((ahora - momento_apagado) < 1500) {
                sys_control_aiot_set_backlight(0);
                return; 
            }
        }

        if (!estado_actual_on) {
            sys_control_aiot_set_backlight(100); 
            estado_actual_on = true;
            // ESP_LOGI(TAG, "Pantalla ON");
        }
    }
    else 
    {
        // APAGADO
        if (estado_actual_on) {
            momento_apagado = esp_timer_get_time() / 1000;
            sys_control_aiot_set_backlight(0); 
            estado_actual_on = false;
            // ESP_LOGI(TAG, "Pantalla OFF");
        }
    }
}