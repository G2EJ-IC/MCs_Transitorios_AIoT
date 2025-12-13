#include "IO_AIoT.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"  
#include "esp_timer.h" 

static const char *TAG = "IO_AIOT";

// --- CONFIGURACION HARDWARE ---
#define LCD_BK_LIGHT_PIN    2  

// PWM Config
#define PWM_FREQ_HZ            5000 
#define PWM_RES_BIT            LEDC_TIMER_10_BIT
#define PWM_CHANNEL            LEDC_CHANNEL_0
#define PWM_TIMER              LEDC_TIMER_0
#define PWM_MODE               LEDC_LOW_SPEED_MODE

// --- VARIABLES ---
static int32_t g_current_brightness_percent = 100;
static uint32_t g_suspension_timeout_ms = 0; // 0 = Desactivado
static bool g_is_dimmed = false;

// --- FUNCION PWM ---
static void apply_pwm_brightness(int percent) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    // Convertir 0-100% a 0-1023
    uint32_t duty = (percent * 1023) / 100;
    
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, duty);
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
}

// --- INICIALIZACION ---
void IO_AIoT_Init(void) {
    ESP_LOGI(TAG, "Init Backlight PWM...");

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = PWM_MODE,
        .duty_resolution  = PWM_RES_BIT,
        .timer_num        = PWM_TIMER,
        .freq_hz          = PWM_FREQ_HZ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = PWM_MODE,
        .channel        = PWM_CHANNEL,
        .timer_sel      = PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LCD_BK_LIGHT_PIN,
        .duty           = 0, 
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    apply_pwm_brightness(100); // Encender al iniciar
}

// --- CONTROL MANUAL (Slider) ---
void IO_Set_Brillo_Manual(int32_t percentage) {
    // Si está suspendido (negro), NO encendemos la luz, solo guardamos el valor.
    if (!g_is_dimmed) {
        if (g_current_brightness_percent != percentage) {
            g_current_brightness_percent = percentage;
            apply_pwm_brightness(percentage);
        }
    } else {
        g_current_brightness_percent = percentage;
    }
}

// --- TIEMPO SUSPENSION (Dropdown) ---
void IO_Set_Tiempo_Suspension(int32_t index) {
    switch (index) {
        case 0: g_suspension_timeout_ms = 0; break;        // Nunca
        case 1: g_suspension_timeout_ms = 15000; break;    // 15s
        case 2: g_suspension_timeout_ms = 30000; break;    // 30s
        case 3: g_suspension_timeout_ms = 60000; break;    // 1 min
        case 4: g_suspension_timeout_ms = 120000; break;   // 2 min
        case 5: g_suspension_timeout_ms = 300000; break;   // 5 min
        default: g_suspension_timeout_ms = 0; break;
    }
}

// --- RELOJ ---
void IO_Get_Uptime(char *buffer, size_t len) {
    int64_t uptime_us = esp_timer_get_time();
    int uptime_sec = uptime_us / 1000000;
    int days = uptime_sec / 86400;
    int hours = (uptime_sec % 86400) / 3600;
    int minutes = (uptime_sec % 3600) / 60;
    int seconds = (uptime_sec % 60);

    if (days > 0) snprintf(buffer, len, "%dd %02d:%02d:%02d", days, hours, minutes, seconds);
    else snprintf(buffer, len, "%02d:%02d:%02d", hours, minutes, seconds);
}

// --- MAQUINA DE ESTADOS (TASK MANAGER) ---
void IO_Task_Manager(void) {
    // Si está en "Nunca" (0), forzamos encendido y salimos
    if (g_suspension_timeout_ms == 0) {
        if (g_is_dimmed) {
            g_is_dimmed = false;
            apply_pwm_brightness(g_current_brightness_percent);
        }
        return;
    }

    // Leer tiempo inactividad LVGL
    uint32_t inactive_ms = lv_display_get_inactive_time(NULL); 

    if (inactive_ms > g_suspension_timeout_ms) {
        // --- APAGAR (SUSPENDER) ---
        if (!g_is_dimmed) {
            g_is_dimmed = true;
            apply_pwm_brightness(0); // <--- AQUI ESTÁ: CERO ABSOLUTO
            ESP_LOGI(TAG, "System suspended (Screen OFF).");
        }
    } else {
        // --- DESPERTAR ---
        if (g_is_dimmed) {
            g_is_dimmed = false;
            apply_pwm_brightness(g_current_brightness_percent);
            ESP_LOGI(TAG, "System Woke Up!");
        }
    }
}