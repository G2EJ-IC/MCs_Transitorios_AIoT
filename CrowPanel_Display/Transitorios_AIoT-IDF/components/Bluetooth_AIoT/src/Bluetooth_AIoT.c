#include "Bluetooth_AIoT.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"

static const char *TAG = "BT_AIOT";
static bool g_bt_connected = false;

// Nombre que verás en el celular
#define DEVICE_NAME "ESP32_AIoT_Device"

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// Callback de eventos GAP (Publicidad / Conexión)
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&adv_params);
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Advertising start failed");
            } else {
                ESP_LOGI(TAG, "Bluetooth Advertising Started");
            }
            break;
        default:
            break;
    }
}

void Bluetooth_AIoT_Init(void) {
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    if (ret) { ESP_LOGE(TAG, "BT Controller Init failed"); return; }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) { ESP_LOGE(TAG, "BT Enable failed"); return; }

    ret = esp_bluedroid_init();
    if (ret) { ESP_LOGE(TAG, "Bluedroid Init failed"); return; }

    ret = esp_bluedroid_enable();
    if (ret) { ESP_LOGE(TAG, "Bluedroid Enable failed"); return; }

    esp_ble_gap_register_callback(gap_event_handler);
    ESP_LOGI(TAG, "Bluetooth Initialized Successfully");
}

void Bluetooth_Start_Advertising(void) {
    // Configurar datos de publicidad
    esp_ble_adv_data_t adv_data = {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = false,
        .min_interval = 0x0006,
        .max_interval = 0x0010,
        .appearance = 0x00,
        .manufacturer_len = 0,
        .p_manufacturer_data =  NULL,
        .service_data_len = 0,
        .p_service_data = NULL,
        .service_uuid_len = 0,
        .p_service_uuid = NULL,
        .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
    };
    esp_ble_gap_config_adv_data(&adv_data);
    esp_ble_gap_set_device_name(DEVICE_NAME);
}

void Bluetooth_Stop(void) {
    esp_ble_gap_stop_advertising();
    ESP_LOGI(TAG, "Bluetooth Stopped");
}

bool Bluetooth_Is_Connected(void) {
    return g_bt_connected; 
    // Nota: Para una lógica completa de conexión, necesitarías implementar 
    // el callback de GATT Server, pero para el UI esto basta por ahora.
}