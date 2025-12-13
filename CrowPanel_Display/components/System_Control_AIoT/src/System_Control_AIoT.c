#include "System_Control_AIoT.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "lvgl.h" 
#include <stdio.h>

static const char *TAG = "System_Control";

#define BL_PIN GPIO_NUM_2 

typedef enum {
    ESTADO_ENCENDIDO,
    ESTADO_ENFRIAMIENTO,
    ESTADO_DORMIDO
} screen_state_t;

static uint32_t time_limit_ms = 15000;      
static int64_t  timestamp_ruido = 0;        
static screen_state_t estado_actual = ESTADO_ENCENDIDO;
static int32_t  brillo_deseado = 100; // Guardamos lo que quiere el usuario

void sys_control_aiot_init(void)
{
    gpio_reset_pin(BL_PIN);
    gpio_set_direction(BL_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(BL_PIN, 1);
    estado_actual = ESTADO_ENCENDIDO;
    ESP_LOGI(TAG, "System Control: Iniciado");
}

// CORRECCIÓN CLAVE: Esta función ahora es inteligente.
// Si actions.cpp grita "¡Prendete!" pero estamos durmiendo, lo ignoramos.
void sys_control_aiot_set_backlight(int32_t percent)
{
    // 1. Guardamos la intención del usuario (para cuando despertemos)
    brillo_deseado = percent;

    // 2. Solo actuamos sobre el Hardware si estamos DESPIERTOS
    if (estado_actual == ESTADO_ENCENDIDO) {
        if (percent > 0) gpio_set_level(BL_PIN, 1);
        else gpio_set_level(BL_PIN, 0); // Manual OFF
    }
    // Si estamos en ENFRIAMIENTO o DORMIDO, ignoramos el comando de hardware
    // para evitar que el bucle de actions.cpp nos despierte.
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
    
    // Al cambiar config, despertamos
    lv_disp_trig_activity(NULL);
    if (estado_actual != ESTADO_ENCENDIDO) {
        gpio_set_level(BL_PIN, 1);
        estado_actual = ESTADO_ENCENDIDO;
    }
}

void sys_control_aiot_manage_sleep(void)
{
    int64_t ahora_us = esp_timer_get_time();
    uint32_t inactividad_ms = lv_disp_get_inactive_time(NULL);

    switch (estado_actual) {
        
        case ESTADO_ENCENDIDO:
            if (inactividad_ms > time_limit_ms) {
                gpio_set_level(BL_PIN, 0); // APAGAR
                estado_actual = ESTADO_ENFRIAMIENTO;
                timestamp_ruido = ahora_us; 
            }
            break;

        case ESTADO_ENFRIAMIENTO:
            // Penalización por ruido (como antes)
            if (inactividad_ms < 100) {
                timestamp_ruido = ahora_us; // Reiniciar cuenta
                return; 
            }
            // Esperar 2 segundos de silencio absoluto
            if ((ahora_us - timestamp_ruido) > 2000000) {
                estado_actual = ESTADO_DORMIDO;
            }
            break;

        case ESTADO_DORMIDO:
            if (inactividad_ms < 100) {
                // AL DESPERTAR: Usamos el brillo que guardamos o forzamos 1
                if (brillo_deseado > 0) gpio_set_level(BL_PIN, 1);
                estado_actual = ESTADO_ENCENDIDO;
            }
            break;
    }
}