#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Inclusiones de Componentes
#include "Configuracion_AIoT.h"
#include "ui.h" // Asegúrate de que el componente UI exporte su include

static const char *TAG = "Main_App";

void app_main(void)
{
    // 1. Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Iniciando Sistema CrowPanel AIoT...");

    // 2. Inicializar Hardware y LVGL
    if (Configuracion_AIoT_Init() != ESP_OK) {
        ESP_LOGE(TAG, "Fallo en Init Hardware. Reiniciando...");
        return;
    }

    // 3. Inicializar Interfaz (EEZ Studio)
    ESP_LOGI(TAG, "Cargando Interfaz de Usuario...");
    ui_init(); 
    
    // 4. Bucle Principal
    while (1) {
        // Mover LVGL (Gráficos)
        uint32_t ms_to_wait = lv_timer_handler();
        
        // Mover EEZ Flow (Lógica)
        ui_tick();
        
        // Dormir para no quemar CPU (Mínimo 5ms)
        if (ms_to_wait < 5) ms_to_wait = 5;
        vTaskDelay(pdMS_TO_TICKS(ms_to_wait));
    }
}