#include "actions.h"
#include "ui.h"
#include "esp_log.h"

// Etiqueta para los logs
static const char *TAG = "UI_ACTIONS";

// Esta función auxiliar se puede dejar vacía por ahora para evitar errores
void ui_reset_wifi_labels() {
    // Futura implementación
}

// --- ACCIÓN BOTÓN: CONNECT ---
void action_fn_connec(lv_event_t * e)
{
    // Solo mostramos un mensaje en el Monitor Serial
    ESP_LOGI(TAG, "--------------------------------------------------");
    ESP_LOGI(TAG, ">> BOTON 'CONNECT' PRESIONADO CORRECTAMENTE <<");
    ESP_LOGI(TAG, "   (La logica WiFi se implementara despues)");
    ESP_LOGI(TAG, "--------------------------------------------------");
    
    // NOTA: No intentamos modificar 'objects.text_area_ssid' aquí todavía
    // para evitar el reinicio si el objeto no ha sido creado por la UI.
}

// --- ACCIÓN BOTÓN: RE-SCAN ---
void action_fn_re_scan(lv_event_t * e)
{
    ESP_LOGI(TAG, ">> BOTON 'RE-SCAN' PRESIONADO <<");
}