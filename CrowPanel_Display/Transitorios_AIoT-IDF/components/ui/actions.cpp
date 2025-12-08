#include "actions.h"
#include "vars.h"
#include "ui.h"
#include "screens.h" 
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

// --- ACCION BOTON: CONNECT / ESCANEAR ---
void action_fn_connec(lv_event_t * e)
{
    // set_var_connec(false); // Ejemplo
    bool inicio = true; // Simulado por ahora

    if (inicio)
    {
        ESP_LOGI(TAG, "Boton Connect presionado");
        if (objects.text_area_ssid) {
            lv_textarea_set_text(objects.text_area_ssid, "Escaneando (Simulado)...");
        }
    }
    else
    {
        ESP_LOGW(TAG, "Escaneo ocupado");
    }
}

// --- ACCION BOTON: RE-SCAN ---
void action_fn_re_scan(lv_event_t * e)
{
    bool inicio = true;

    if (inicio) {
        ESP_LOGI(TAG, "Re-Scan solicitado...");
        if (objects.text_area_ssid) {
            lv_textarea_set_text(objects.text_area_ssid, "Re-escaneando...");
        }
    } else {
        ESP_LOGW(TAG, "Error Re-Scan");
    }
}

#ifdef __cplusplus
}
#endif