#include "actions.h"
#include "ui.h"
#include "vars.h"
#include "screens.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// C Headers
extern "C" {
    #include "WiFi_AIoT.h"
    #include "IO_AIoT.h" 
    // #include "Bluetooth_AIoT.h" // REMOVIDO PARA EVITAR ERRORES
}

static const char *TAG = "UI_ACTIONS";

enum ConnectionMethod {
    METHOD_WIFI_MULTI = 0,
    METHOD_BLUETOOTH  = 1, 
    METHOD_BOTH       = 2
};

// --- AYUDAS VISUALES ---
static void helper_update_visuals() {
    bool is_wifi_connected = get_wifi_is_connected();
    set_var_connec(is_wifi_connected);

    if (is_wifi_connected) {
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, get_wifi_ssid());
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, get_wifi_ip());
        if (objects.ui_lab_dns)  lv_label_set_text(objects.ui_lab_dns, get_wifi_dns());
        if (objects.ui_lab_mac)  lv_label_set_text(objects.ui_lab_mac, get_wifi_mac());
    } else {
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, "Desconectado");
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, "0.0.0.0");
    }

    lv_color_t color_status = (is_wifi_connected) ? lv_color_hex(0x008000) : lv_color_hex(0xFF0000);
    if (objects.bt_conectado_main3_tab1) lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, color_status, LV_PART_MAIN);
    if (objects.bt_conectado_main3_tab2) lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab2, color_status, LV_PART_MAIN);
}

static void helper_perform_connect() {
    char ssid_buffer[64] = {0};
    if (objects.text_area_ssid) { 
        lv_dropdown_get_selected_str(objects.text_area_ssid, ssid_buffer, sizeof(ssid_buffer));
        set_var_text_area_ssid_value(ssid_buffer);
    }
    const char *pass_ptr = "";
    if (objects.text_area_password) {
        pass_ptr = lv_textarea_get_text(objects.text_area_password);
        set_var_text_area_pass_value(pass_ptr);
    }

    int32_t method = 0; 
    if (objects.drop_down_1) method = lv_dropdown_get_selected(objects.drop_down_1);

    if (method == METHOD_WIFI_MULTI || method == METHOD_BOTH) {
        if (strlen(ssid_buffer) > 0) wifi_connect(ssid_buffer, (char*)pass_ptr);
    }
    vTaskDelay(pdMS_TO_TICKS(200)); 
    helper_update_visuals();
}

static void event_keyboard_ready_cb(lv_event_t * e) {
    helper_perform_connect();
}

// --- ACCIONES EEZ ---
void action_fn_connec_aio_t(lv_event_t * e) { helper_perform_connect(); }

void action_fn_connec(lv_event_t * e) {
    int32_t method = 0;
    if (objects.drop_down_1) method = lv_dropdown_get_selected(objects.drop_down_1);

    if (method == METHOD_WIFI_MULTI || method == METHOD_BOTH) {
        char *list = wifi_scan_networks_get_list();
        if (list && objects.text_area_ssid) {
            lv_dropdown_set_options(objects.text_area_ssid, list);
            free(list);
        }
    }
    static bool tk_linked = false;
    if (!tk_linked && objects.keyboard) {
        lv_obj_add_event_cb(objects.keyboard, event_keyboard_ready_cb, LV_EVENT_READY, NULL);
        tk_linked = true;
    }
    helper_update_visuals();
}

void action_fn_re_scan(lv_event_t * e) {
    set_var_re_scan(true);
    action_fn_connec(e);
    set_var_re_scan(false);
}

void action_fn_update_suspension(lv_event_t * e) {}

// --- TAREA PERIODICA ---
extern "C" void ui_update_periodic_task(void)
{
    static uint32_t last_clock_update = 0;
    static uint32_t last_wifi_update = 0;
    static int32_t  last_suspension_index = -1; 

    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

    // 1. Reloj
    if ((now - last_clock_update) >= 1000) {
        char time_str[32];
        IO_Get_Uptime(time_str, sizeof(time_str));
        set_var_label_dhms_1(time_str);
        set_var_label_dhms_2(time_str);
        if (objects.label_dhms_1) lv_label_set_text(objects.label_dhms_1, time_str);
        if (objects.label_dhms_2) lv_label_set_text(objects.label_dhms_2, time_str);
        last_clock_update = now;
    }

    // 2. Estado Wifi
    if ((now - last_wifi_update) >= 500) {
        helper_update_visuals();
        last_wifi_update = now;
    }

    // 3. Sincronizar Slider Brillo
    IO_Set_Brillo_Manual(get_var_slider_porcentaje());

    // 4. Sincronizar Dropdown Suspensión
    int32_t current_index = 0;
    if (objects.drop_down_suspender) {
        current_index = lv_dropdown_get_selected(objects.drop_down_suspender);
    } else {
        current_index = get_var_drop_down_suspender();
    }

    if (current_index != last_suspension_index) {
        IO_Set_Tiempo_Suspension(current_index);
        last_suspension_index = current_index;
    }
}