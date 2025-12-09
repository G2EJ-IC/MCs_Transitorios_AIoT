#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init_sta(void);
char* wifi_scan_networks_get_list(void);
void wifi_connect(const char *ssid, const char *password);

// Getters
bool get_wifi_is_connected(void);
char* get_wifi_ssid(void);
char* get_wifi_ip(void);
char* get_wifi_dns(void); // <--- AGREGADO
char* get_wifi_mac(void);

#ifdef __cplusplus
}
#endif