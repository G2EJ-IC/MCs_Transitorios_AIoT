/* main/main_AIoT.c */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Componentes AIoT Modulares
#include "Display_AIoT.h"
#include "Input_AIoT.h"

// Interfaz de Usuario (EEZ Studio)
#include "ui/src/ui.h"

static const char *TAG = "Main_AIoT";

// Tarea periódica para informar a LVGL del paso del tiempo
static void lvgl_tick_task(void *arg)
{
    lv_tick_inc(AIOT_LVGL_TICK_PERIOD_MS);
}

void app_main(void)
{
    // 1. Inicialización de NVS (Requerido para calibración WiFi/Touch persistente)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES |

        | ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Iniciando Sistema CrowPanel AIoT Modular...");

    // 2. Inicializar HAL de Pantalla y LVGL
    Display_AIoT_Init();

    // 3. Inicializar HAL de Entrada Táctil
    Input_AIoT_Init();

    // 4. Configurar Timer de Alta Precisión para LVGL Tick
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvgl_tick_task,
        .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, AIOT_LVGL_TICK_PERIOD_MS * 1000));

    // 5. Inicializar UI de EEZ Studio
    // Esta función configura los widgets, estilos y eventos definidos en el editor.
    ui_init();

    ESP_LOGI(TAG, "Sistema Operativo. Ejecutando Bucle Principal de UI.");

    // 6. Super Bucle (Task Loop)
    // En LVGL, este bucle maneja las tareas, animaciones y eventos de entrada.
    while (1)
    {
        // Ejecutar motor de LVGL
        // Retorna el tiempo en ms hasta la próxima tarea necesaria.
        uint32_t time_till_next = lv_timer_handler();

        // Tick de lógica UI de EEZ (para flujos visuales)
        ui_tick();

        // Gestión de energía: Dormir para liberar CPU
        // Limitamos el sueño a un máximo razonable para mantener la respuesta.
        if (time_till_next > 50)
            time_till_next = 50;

        vTaskDelay(pdMS_TO_TICKS(time_till_next));
    }
}
