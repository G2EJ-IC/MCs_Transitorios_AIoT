#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

// COMPONENTES
#include "Configuracion_AIoT.h"
#include "WiFi_AIoT.h"
#include "System_Control_AIoT.h"
#include "ui.h" 
#include "lvgl.h"

// Declaración externa
extern void ui_update_periodic_task(void);

static const char *TAG = "Main_App";

void app_main(void)
{
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    // Inicializar Hardware
    if (Configuracion_AIoT_Init() != ESP_OK) return;
    sys_control_aiot_init();
    wifi_init_sta();
    ui_init(); 
    
    // --- SOLUCIÓN AL REINICIO ---
    // Registramos esta tarea en el Watchdog para poder alimentarlo
    esp_task_wdt_add(NULL);

    for (;;) {
        // 1. LVGL (Gráficos)
        uint32_t time_until_next = lv_timer_handler();
        
        // 2. EEZ Flow
        ui_tick();
        
        // 3. Lógica (Reloj + Slider)
        ui_update_periodic_task();
        
        // 4. ALIMENTAR AL PERRO (Evita el reinicio)
        esp_task_wdt_reset();

        // 5. Delay Inteligente (Evita saturar la CPU)
        // Si LVGL dice "espera 500ms", nosotros esperamos máximo 10ms
        // para mantener el sistema fluido y responder al touch.
        if (time_until_next > 10) time_until_next = 10;
        if (time_until_next < 1) time_until_next = 1;

        vTaskDelay(pdMS_TO_TICKS(time_until_next));
    }
}