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
    #include "IO_AIoT.h" 
    #include "Bluetooth_AIoT.h" // <--- NUEVO INCLUDE
}

static const char *TAG = "UI_ACTIONS";

enum MetodoConexion {
    METODO_WIFI_MULTI = 0,
    METODO_BLUETOOTH  = 1,
    METODO_AMBOS      = 2
};

// -------------------------------------------------------------------------
// 1. FUNCIONES AUXILIARES
// -------------------------------------------------------------------------
static void helper_update_visuals() {
    bool is_wifi_connected = get_wifi_is_connected();
    bool is_bt_connected = Bluetooth_Is_Connected(); // Consultamos estado BT
    
    // Actualizamos la variable global de conexión (lógica OR: si uno está, estamos conectados)
    set_var_connec(is_wifi_connected || is_bt_connected);

    // --- WIFI VISUALS ---
    if (is_wifi_connected) {
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, get_wifi_ssid());
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, get_wifi_ip());
        if (objects.ui_lab_dns)  lv_label_set_text(objects.ui_lab_dns, get_wifi_dns());
        if (objects.ui_lab_mac)  lv_label_set_text(objects.ui_lab_mac, get_wifi_mac());
    } else {
        if (objects.ui_lab_ssid) lv_label_set_text(objects.ui_lab_ssid, "Desconectado");
        if (objects.ui_lab_ip)   lv_label_set_text(objects.ui_lab_ip, "0.0.0.0");
    }

    // --- INDICADORES LED EN PANTALLA ---
    // Verde si hay alguna conexión válida
    lv_color_t color_status = (is_wifi_connected || is_bt_connected) ? lv_color_hex(0x008000) : lv_color_hex(0xFF0000);

    if (objects.bt_conectado_main3_tab1) 
        lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, color_status, LV_PART_MAIN);
    if (objects.bt_conectado_main3_tab2) 
        lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab2, color_status, LV_PART_MAIN);
}

static void helper_perform_connect() {
    char ssid_buffer[64] = {0};
    // Leer SSID
    if (objects.text_area_ssid) { 
        lv_dropdown_get_selected_str(objects.text_area_ssid, ssid_buffer, sizeof(ssid_buffer));
        set_var_text_area_ssid_value(ssid_buffer);
    }

    // Leer Password
    const char *pass_ptr = "";
    if (objects.text_area_password) {
        pass_ptr = lv_textarea_get_text(objects.text_area_password);
        set_var_text_area_pass_value(pass_ptr);
    }

    // Leer Método de Conexión
    int32_t metodo = 0; 
    if (objects.drop_down_1) { 
        metodo = lv_dropdown_get_selected(objects.drop_down_1);
        set_var_drop_down_metodo(metodo);
    } else {
        metodo = get_var_drop_down_metodo();
    }

    ESP_LOGI(TAG, "Conexión Solicitada. Método: %d", (int)metodo);

    // --- LÓGICA DE CONEXIÓN ---
    
    // 1. Manejo WiFi
    if (metodo == METODO_WIFI_MULTI || metodo == METODO_AMBOS) {
        if (strlen(ssid_buffer) > 0) {
            wifi_connect(ssid_buffer, (char*)pass_ptr);
        } else {
            ESP_LOGW(TAG, "SSID vacío. No se puede conectar WiFi.");
        }
    }

    // 2. Manejo Bluetooth
    if (metodo == METODO_BLUETOOTH || metodo == METODO_AMBOS) {
        // Al "conectar" Bluetooth, iniciamos el Advertising para ser visibles
        Bluetooth_Start_Advertising();
    } else {
        // Si seleccionó solo WiFi, podríamos apagar BT para ahorrar
        // Bluetooth_Stop(); 
    }

    vTaskDelay(pdMS_TO_TICKS(200)); 
    helper_update_visuals();
}

static void event_keyboard_ready_cb(lv_event_t * e) {
    helper_perform_connect();
}

// -------------------------------------------------------------------------
// 2. ACCIONES EEZ STUDIO
// -------------------------------------------------------------------------

void action_fn_connec_aio_t(lv_event_t * e) {
    helper_perform_connect();
}

void action_fn_connec(lv_event_t * e) {
    // Escaneo solo si es modo WiFi o Ambos
    int32_t metodo = 0;
    if (objects.drop_down_1) metodo = lv_dropdown_get_selected(objects.drop_down_1);

    if (metodo == METODO_WIFI_MULTI || metodo == METODO_AMBOS) {
        char *lista = wifi_scan_networks_get_list();
        if (lista && objects.text_area_ssid) {
            lv_dropdown_set_options(objects.text_area_ssid, lista);
            free(lista);
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

void action_fn_update_suspension(lv_event_t * e) {
    // Vacío intencionalmente (polling directo)
}

// -------------------------------------------------------------------------
// 3. TAREA PERIÓDICA
// -------------------------------------------------------------------------
extern "C" void ui_update_periodic_task(void)
{
    static uint32_t last_clock_update = 0;
    static uint32_t last_wifi_update = 0;
    static int32_t  last_suspension_index = -1; 

    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

    // --- A. RELOJ (Cada 1000 ms) ---
    if ((now - last_clock_update) >= 1000) {
        char time_str[32];
        IO_Get_Uptime(time_str, sizeof(time_str));
        set_var_label_dhms_1(time_str);
        set_var_label_dhms_2(time_str);
        if (objects.label_dhms_1) lv_label_set_text(objects.label_dhms_1, time_str);
        if (objects.label_dhms_2) lv_label_set_text(objects.label_dhms_2, time_str);
        last_clock_update = now;
    }

    // --- B. ESTADO CONEXIÓN (Cada 500 ms) ---
    if ((now - last_wifi_update) >= 500) {
        helper_update_visuals();
        last_wifi_update = now;
    }

    // --- C. SINCRONIZACIÓN DIRECTA (Slider y DropDown) ---
    IO_Set_Brillo_Manual(get_var_slider_porcentaje());

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