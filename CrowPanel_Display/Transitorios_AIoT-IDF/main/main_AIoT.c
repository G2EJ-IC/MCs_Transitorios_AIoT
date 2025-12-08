#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Inclusiones de nuestros componentes
#include "Configuracion_AIoT.h"
#include "src/ui.h" // Cabecera generada por EEZ Studio

static const char *TAG = "Main_App";

void app_main(void)
{
    ESP_LOGI(TAG, "Iniciando Sistema CrowPanel AIoT...");

    // 1. Inicialización de Hardware
    if (Configuracion_AIoT_Init()!= ESP_OK) {
        ESP_LOGE(TAG, "Error Crítico en HAL. Sistema Detenido.");
        while(1) vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // 2. Inicialización de UI (EEZ Flow)
    // Esta función configura las pantallas, estilos y acciones definidos en EEZ Studio
    ui_init(); 
    
    ESP_LOGI(TAG, "Interfaz Gráfica Iniciada. Entrando en bucle de eventos.");

    // 3. Bucle Principal
    while (1) {
        // Manejador de Tareas LVGL
        // Devuelve el tiempo en ms hasta la próxima tarea requerida
        uint32_t time_till_next = lv_timer_handler();
        
        // Manejador de Lógica de Flujo EEZ (Animaciones, cambios de estado)
        ui_tick(); 

        // Gestión de energía: Dormir lo necesario
        if (time_till_next == 0) time_till_next = 1;
        // Capar el tiempo de espera máximo para mantener reactividad (ej. 5ms)
        if (time_till_next > 5) time_till_next = 5;
        
        vTaskDelay(pdMS_TO_TICKS(time_till_next));
    }
}
