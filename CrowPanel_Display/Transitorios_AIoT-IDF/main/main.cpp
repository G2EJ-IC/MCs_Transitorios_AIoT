#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

// Solo necesitamos el servicio de Display
#include "Display_AIoT.h"
#include "IO_AIoT.h" 

static const char *TAG = "MAIN";

// Objetos Globales
display_service display;
io_service io;

extern "C" void app_main(void)
{
    // 1. Iniciar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Iniciar Servicios
    ESP_LOGI(TAG, "Iniciando Sistema...");
    display.setup(); 
    io.setup();

    // 3. Bucle UI
    while (1) {
        display.loop(); // Mueve LVGL y EEZ
        io.loop();      // Por si necesitas lógica extra
        
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
