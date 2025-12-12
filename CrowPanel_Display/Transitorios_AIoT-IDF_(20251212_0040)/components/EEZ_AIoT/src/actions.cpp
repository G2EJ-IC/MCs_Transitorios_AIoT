#include "actions.h"
#include "ui.h"
#include "screens.h"
#include "vars.h" // Para get_var_slider_porcentaje
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

// Includes de ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// COMPONENTES (C Linkage)
extern "C" {
    #include "WiFi_AIoT.h"
    #include "System_Control_AIoT.h" // <--- TU NUEVO HEADER
}

static const char *TAG = "UI_ACTIONS";

// --- FUNCIÓN DE CICLO (Se llama desde el main) ---
// Esta función actualiza el Reloj y lee el Slider constantemente
extern "C" void ui_update_periodic_task(void)
{
    // 1. Actualizar Cronómetro (DHMS)
    char time_str[32];
    sys_control_aiot_get_uptime(time_str, sizeof(time_str));
    
    // Actualizar labels si existen en la pantalla actual
    if (objects.label_dhms_1) lv_label_set_text(objects.label_dhms_1, time_str);
    if (objects.label_dhms_2) lv_label_set_text(objects.label_dhms_2, time_str);

    // 2. Control de Brillo (Slider)
    // Leemos la variable nativa de EEZ (SliderPorcentaje)
    int32_t brillo = get_var_slider_porcentaje();
    
    // Enviamos al hardware
    sys_control_aiot_set_backlight(brillo);
}

// --- RESTO DE TU CÓDIGO WIFI (Igual que antes) ---

static void update_ui_wifi_data() 
{
    if (get_wifi_is_connected()) {
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, get_wifi_ssid());
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, get_wifi_ip());
        if (objects.ui_lab_dns)  lv_label_set_text(objects.ui_lab_dns, get_wifi_dns()); 
        if (objects.ui_lab_mac)  lv_label_set_text(objects.ui_lab_mac, get_wifi_mac());

        if (objects.bt_conectado_main3_tab1) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, lv_color_hex(0x008000), LV_PART_MAIN | LV_STATE_DEFAULT);
        if (objects.bt_conectado_main3_tab2) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab2, lv_color_hex(0x008000), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, "Nombre WiFi (SSID)");
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, "xx.xx.xx.xx");
        if (objects.ui_lab_dns)  lv_label_set_text(objects.ui_lab_dns, "xx.xx.xx.xx");
        if (objects.ui_lab_mac)  lv_label_set_text(objects.ui_lab_mac, "xx:xx:xx:xx:xx:xx");

        if (objects.bt_conectado_main3_tab1) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        if (objects.bt_conectado_main3_tab2) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab2, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

static void on_keyboard_ready_cb(lv_event_t * e) {
    char ssid[33] = {0};
    if (objects.text_area_ssid) lv_dropdown_get_selected_str(objects.text_area_ssid, ssid, sizeof(ssid));
    
    const char *pass = "";
    if (objects.text_area_password) pass = lv_textarea_get_text(objects.text_area_password);
    
    if (strlen(ssid) > 0) {
        wifi_connect(ssid, pass);
        // Pequeño delay visual
        vTaskDelay(pdMS_TO_TICKS(100)); 
        update_ui_wifi_data();
    }
}

void action_fn_connec(lv_event_t * e) 
{
    char *list = wifi_scan_networks_get_list();
    if (list && objects.text_area_ssid) {
        lv_dropdown_set_options(objects.text_area_ssid, list);
        lv_dropdown_open(objects.text_area_ssid);
        free(list);
    }
    
    static bool keyb_linked = false;
    if (!keyb_linked && objects.keyboard) {
        lv_obj_add_event_cb(objects.keyboard, on_keyboard_ready_cb, LV_EVENT_READY, NULL);
        keyb_linked = true;
    }
    update_ui_wifi_data();
}

void action_fn_connec_aio_t(lv_event_t * e) 
{
    action_fn_connec(e);
}

void action_fn_re_scan(lv_event_t * e) {
    action_fn_connec(e);
}

void ui_reset_wifi_labels() {
    update_ui_wifi_data();
}