#include "actions.h"
#include "ui.h"
#include "vars.h"
#include "screens.h"

// Headers del Sistema (ESP-IDF)
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Headers C Linkage (Tus librerías propias)
extern "C" {
    #include "WiFi_AIoT.h"
    #include "System_Control_AIoT.h"
    // #include "Bluetooth_AIoT.h" // Descomentar a futuro
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

    // Actualizamos las variables globales de EEZ
    // EEZ Studio se encarga de refrescar la pantalla SOLO si el texto cambia
    if (is_connected) {
        set_var_ui_lab_ssid(get_wifi_ssid());
        set_var_ui_lab_ip(get_wifi_ip());
        set_var_ui_lab_dns(get_wifi_dns());
        set_var_ui_lab_mac(get_wifi_mac());
        set_var_connec(true);
    } else {
        set_var_ui_lab_ssid("Desconectado");
        set_var_ui_lab_ip("0.0.0.0");
        set_var_connec(false);
    }

    // Actualización manual de colores (Backup visual)
    if (is_connected) {
        if (objects.bt_conectado_main3_tab1) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, lv_color_hex(0x008000), LV_PART_MAIN);
    } else {
        if (objects.bt_conectado_main3_tab1) 
            lv_obj_set_style_bg_color(objects.bt_conectado_main3_tab1, lv_color_hex(0xFF0000), LV_PART_MAIN);
    }
}

// -------------------------------------------------------------------------
// 2. LÓGICA DE CONEXIÓN
// -------------------------------------------------------------------------
static void helper_perform_connect() {
    int32_t metodo = get_var_drop_down_metodo();
    const char *ssid = get_var_text_area_ssid_value();
    const char *pass = get_var_text_area_pass_value();

    ESP_LOGI(TAG, "Conexión Solicitada. Método: %d", (int)metodo);

    // Lógica WiFi
    if (metodo == METODO_WIFI_MULTI || metodo == METODO_AMBOS) {
        if (ssid != NULL && strlen(ssid) > 0) {
            ESP_LOGI(TAG, "Conectando WiFi a: %s", ssid);
            wifi_connect((char*)ssid, (char*)pass);
        } else {
            ESP_LOGW(TAG, "SSID vacío. Omitiendo.");
        }
    }
    
    // Lógica Bluetooth
    if (metodo == METODO_BLUETOOTH || metodo == METODO_AMBOS) {
        // bt_connect_logic(); 
    }

    // Espera breve y actualización forzada inmediata
    vTaskDelay(pdMS_TO_TICKS(200)); 
    helper_update_visuals();
}

static void event_keyboard_ready_cb(lv_event_t * e) {
    helper_perform_connect();
}

// -------------------------------------------------------------------------
// 3. ACCIONES (Eventos EEZ)
// -------------------------------------------------------------------------

// Botón Rojo/Verde
void action_fn_connec_aio_t(lv_event_t * e) {
    helper_perform_connect();
}

// Botón Azul
void action_fn_connec(lv_event_t * e) {
    int32_t metodo = get_var_drop_down_metodo();
    
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

// Botón Re-Scan
void action_fn_re_scan(lv_event_t * e) {
    set_var_re_scan(true);
    action_fn_connec(e);
    set_var_re_scan(false);
}

// -------------------------------------------------------------------------
// 4. TAREA PERIÓDICA (CON CONTROL DE TIEMPO)
// -------------------------------------------------------------------------
extern "C" void ui_update_periodic_task(void)
{
    // Variables estáticas para llevar el tiempo (Contadores Internos)
    static uint32_t last_clock_update = 0;
    static uint32_t last_wifi_update = 0;
    
    // Tiempo actual del sistema en Milisegundos
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

    // --- A. Actualizar RELOJ (Cada 1000 ms = 1 segundo) ---
    if ((now - last_clock_update) >= 1000) {
        char time_str[32];
        sys_control_aiot_get_uptime(time_str, sizeof(time_str));
        
        // Enviamos el tiempo a las variables de EEZ (Label DHMS)
        set_var_label_dhms_1(time_str);
        set_var_label_dhms_2(time_str);
        
        last_clock_update = now;
    }

    // --- B. Actualizar ESTADO WIFI (Cada 500 ms) ---
    // Esto evita que el botón parpadee o que el ESP se sature leyendo WiFi
    if ((now - last_wifi_update) >= 500) {
        helper_update_visuals();
        last_wifi_update = now;
    }

    // --- C. Actualizar Brillo (Siempre, o también se puede temporizar) ---
    // El brillo no suele causar parpadeo visual en la UI, se puede dejar directo
    sys_control_aiot_set_backlight(get_var_slider_porcentaje());
}