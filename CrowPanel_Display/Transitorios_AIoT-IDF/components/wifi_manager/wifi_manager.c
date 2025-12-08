#include "wifi_manager.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "WIFI_MANAGER";
static bool scan_done = false;
static uint16_t ap_count = 0;
static wifi_ap_record_t *ap_list = NULL;

#include "esp_event.h"

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE) {
        uint16_t number = 0;
        esp_wifi_scan_get_ap_num(&number);
        ap_count = number;
        if (ap_list) {
            free(ap_list);
        }
        ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * number);
        if (ap_list) {
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_list));
        }
        scan_done = true;
        ESP_LOGI(TAG, "Scan done: %d networks found", number);
    }
}

void wifi_init_global(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

uint16_t wifi_get_result_count(void) {
    return ap_count;
}

wifi_ap_record_t *wifi_get_results(void) {
    return ap_list;
}

bool wifi_scan_finished(void) {
    return scan_done;
}

void wifi_scan_reset_flag(void) {
    scan_done = false;
}

bool wifi_start_scan_async(void) {
    wifi_scan_config_t scan_config = {0};
    scan_config.show_hidden = true;
    esp_err_t err = esp_wifi_scan_start(&scan_config, false);
    if (err == ESP_OK) {
        scan_done = false;
        return true;
    }
    ESP_LOGE(TAG, "Failed to start scan: %s", esp_err_to_name(err));
    return false;
}
