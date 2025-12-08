#include "actions.h"
#include "vars.h"
#include "ui.h"
#include "screens.h" // <--- ESTO FALTABA Y CAUSABA EL ERROR 'objects not declared'

// ESP-IDF Includes
#include "esp_log.h"

static const char *TAG = "UI_ACTIONS";

#ifdef __cplusplus
extern "C" {
#endif

// Helper para resetear textos
static void ui_reset_wifi_labels()
{
    if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, "Nombre WiFi (SSID)");
    if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip,   "xx.xx.xx.xx");
    if (objects.ui_lab_dns)  lv_label_set_text(objects.ui_lab_dns,  "xx.xx.xx.xx");
    if (objects.ui_lab_mac)  lv_label_set_text(objects.ui_lab_mac,  "xx:xx:xx:xx:xx:xx");
}

// --- ACCIÓN BOTÓN: CONNECT / ESCANEAR ---
void action_fn_connec(lv_event_t * e)
{
    // Actualizar variable de EEZ
    set_var_connec(false);

    // Iniciar escaneo asíncrono (C puro)
    bool inicio = wifi_start_scan_async();

    if (inicio)
    {
        ESP_LOGI(TAG, "Boton Connect: Iniciando escaneo...");
        
        // Feedback en pantalla
        if (objects.text_area_ssid) {
            lv_textarea_set_text(objects.text_area_ssid, "Escaneando...");
        }

        // Banner en Log
        ESP_LOGI(TAG, "=========================================================");
        ESP_LOGI(TAG, " Universidad Distrital Francisco Jose de Caldas");
        ESP_LOGI(TAG, " Facultad Tecnologica | Maestria en Ingenieria Civil");
        ESP_LOGI(TAG, " Monitoreo AIoT | @2025");
        ESP_LOGI(TAG, "=========================================================");
    }
    else
    {
        ESP_LOGW(TAG, "Escaneo ocupado, intente luego.");
    }
}

// --- ACCIÓN BOTÓN: RE-SCAN ---
void action_fn_re_scan(lv_event_t * e)
{
    bool inicio = wifi_start_scan_async();

    if (inicio) {
        ESP_LOGI(TAG, "Re-Scan solicitado...");
        if (objects.text_area_ssid) {
            lv_textarea_set_text(objects.text_area_ssid, "Buscando...");
        }
    } else {
        ESP_LOGW(TAG, "Sistema ocupado.");
    }
}

// --- ACCIÓN TECLADO ---
void action_fn_teclado_ready(lv_event_t * e)
{
    // Aquí puedes agregar lógica al terminar de escribir
}

#ifdef __cplusplus
}
#endif
