#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Display_AIoT.h"

void app_main(void)
{
    // 1. Inicializar Hardware (Lógica encapsulada en Configuracion_AIoT)
    Init_Display_AIoT();

    // 2. Bucle Principal
    while (1) {
        // El sistema está vivo. Aquí irá tu lógica futura.
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}