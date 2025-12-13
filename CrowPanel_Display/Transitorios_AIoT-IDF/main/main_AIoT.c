#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

// COMPONENTES
#include "Configuracion_AIoT.h"
#include "WiFi_AIoT.h"
#include "IO_AIoT.h"
#include "Bluetooth_AIoT.h" // <--- NUEVO INCLUDE
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
    
    // Inicializar Hardware General
    if (Configuracion_AIoT_Init() != ESP_OK) return;
    
    IO_AIoT_Init();        // Pantalla y Energía
    Bluetooth_AIoT_Init(); // <--- NUEVO: Inicializar pila Bluetooth
    wifi_init_sta();       // WiFi
    
    ui_init(); 
    
    esp_task_wdt_add(NULL);

    for (;;) {
        // 1. LVGL
        uint32_t time_until_next = lv_timer_handler();
        
        // 2. EEZ Flow
        ui_tick();
        
        // 3. Lógica UI (Reloj, Wifi, Slider)
        ui_update_periodic_task();
        
        // 4. Watchdog
        esp_task_wdt_reset();

        // 5. Delay Inteligente
        if (time_until_next > 10) time_until_next = 10;
        if (time_until_next < 1) time_until_next = 1;

        // 6. GESTOR DE ENERGÍA
        IO_Task_Manager();

        vTaskDelay(pdMS_TO_TICKS(time_until_next));
    }
}