#include "actions.h"
#include "ui.h"
#include "vars.h"
#include "screens.h"

// Headers del Sistema
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Headers C Linkage
extern "C" {
    #include "WiFi_AIoT.h"
    #include "System_Control_AIoT.h"
    // #include "Bluetooth_AIoT.h" 
}

static const char *TAG = "UI_ACTIONS";

// --- Enums ---
enum MetodoConexion {
    METODO_WIFI_MULTI = 0,
    METODO_BLUETOOTH  = 1,
    METODO_AMBOS      = 2
};

// -------------------------------------------------------------------------
// 1. FUNCIONES AUXILIARES (Actualización Visual)
// -------------------------------------------------------------------------
static void helper_update_visuals() {
    bool is_connected = get_wifi_is_connected();

    set_var_connec(is_connected);

    // --- FORZAMOS LA ACTUALIZACIÓN VISUAL (LVGL) ---
    if (is_connected) {
        // Textos
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, get_wifi_ssid());
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, get_wifi_ip());
        if (objects.ui_lab_dns)  lv_label_set_text(objects.ui_lab_dns, get_wifi_dns());
        if (objects.ui_lab_mac)  lv_label_set_text(objects.ui_lab_mac, get_wifi_mac());
        
        // --- Color Verde (Conectado) ---
        // BOTÓN TAB 1
        if (objects.bt_conectado_main3_tab1) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, lv_color_hex(0x008000), LV_PART_MAIN);
        
        // BOTÓN TAB 2
        if (objects.bt_conectado_main3_tab2) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab2, lv_color_hex(0x008000), LV_PART_MAIN);
            
    } else {
        // Textos
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, "Desconectado");
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, "0.0.0.0");
        
        // --- Color Rojo (Desconectado) ---
        // BOTÓN TAB 1
        if (objects.bt_conectado_main3_tab1) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, lv_color_hex(0xFF0000), LV_PART_MAIN);

        // BOTÓN TAB 2
        if (objects.bt_conectado_main3_tab2) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab2, lv_color_hex(0xFF0000), LV_PART_MAIN);
    }
}

// -------------------------------------------------------------------------
// 2. LÓGICA DE CONEXIÓN (Lectura UI -> Ejecución)
// -------------------------------------------------------------------------
static void helper_perform_connect() {
    // A. LEER LA PANTALLA
    
    // 1. Leer SSID del Dropdown
    char ssid_buffer[64] = {0};
    if (objects.text_area_ssid) { 
        lv_dropdown_get_selected_str(objects.text_area_ssid, ssid_buffer, sizeof(ssid_buffer));
        set_var_text_area_ssid_value(ssid_buffer);
    }

    // 2. Leer Password del TextArea
    const char *pass_ptr = "";
    if (objects.text_area_password) {
        pass_ptr = lv_textarea_get_text(objects.text_area_password);
        set_var_text_area_pass_value(pass_ptr);
    }

    // 3. Leer Método (Dropdown)
    int32_t metodo = 0; // Default WiFi
    
    if (objects.drop_down_1) { 
        metodo = lv_dropdown_get_selected(objects.drop_down_1);
        set_var_drop_down_metodo(metodo);
    } else {
        metodo = get_var_drop_down_metodo();
    }

    ESP_LOGI(TAG, "Conexión Solicitada. Método: %d | SSID: %s", (int)metodo, ssid_buffer);

    // B. LÓGICA DE CONEXIÓN
    if (metodo == METODO_WIFI_MULTI || metodo == METODO_AMBOS) {
        if (strlen(ssid_buffer) > 0) {
            wifi_connect(ssid_buffer, (char*)pass_ptr);
        } else {
            ESP_LOGW(TAG, "SSID vacío. No se puede conectar.");
        }
    }
    
    if (metodo == METODO_BLUETOOTH || metodo == METODO_AMBOS) {
        // bt_connect();
    }

    vTaskDelay(pdMS_TO_TICKS(200)); 
    helper_update_visuals();
}

static void event_keyboard_ready_cb(lv_event_t * e) {
    helper_perform_connect();
}

// -------------------------------------------------------------------------
// 3. ACCIONES (Eventos EEZ)
// -------------------------------------------------------------------------

void action_fn_connec_aio_t(lv_event_t * e) {
    helper_perform_connect();
}

void action_fn_connec(lv_event_t * e) {
    char *lista = wifi_scan_networks_get_list();
    if (lista && objects.text_area_ssid) {
        lv_dropdown_set_options(objects.text_area_ssid, lista);
        free(lista);
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

// -------------------------------------------------------------------------
// 4. TAREA PERIÓDICA
// -------------------------------------------------------------------------
extern "C" void ui_update_periodic_task(void)
{
    static uint32_t last_clock_update = 0;
    static uint32_t last_wifi_update = 0;
    
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

    // --- A. RELOJ (Cada 1000 ms) ---
    if ((now - last_clock_update) >= 1000) {
        char time_str[32];
        sys_control_aiot_get_uptime(time_str, sizeof(time_str));
        
        set_var_label_dhms_1(time_str);
        set_var_label_dhms_2(time_str);

        if (objects.label_dhms_1) lv_label_set_text(objects.label_dhms_1, time_str);
        if (objects.label_dhms_2) lv_label_set_text(objects.label_dhms_2, time_str);
        
        last_clock_update = now;
    }

    // --- B. WIFI CHECK (Cada 500 ms) ---
    if ((now - last_wifi_update) >= 500) {
        helper_update_visuals();
        last_wifi_update = now;
    }

    // --- C. BRILLO ---
    sys_control_aiot_set_backlight(get_var_slider_porcentaje());
}