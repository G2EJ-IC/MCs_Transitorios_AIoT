#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init_global(void);
uint16_t wifi_get_result_count(void);
wifi_ap_record_t *wifi_get_results(void);
bool wifi_scan_finished(void);
void wifi_scan_reset_flag(void);
bool wifi_start_scan_async(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H
