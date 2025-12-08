#ifndef __SCANWIFI_AIOT_H__
#define __SCANWIFI_AIOT_H__

#pragma once

#include <Arduino.h>
#include <WiFi.h>

class ScanWiFi_AIoT {
public:
    static const int MAX_NETWORKS = 20;

    ScanWiFi_AIoT();

    void begin();

    // Inicia el escaneo pero NO espera. Devuelve true si inició correctamente.
    bool iniciarEscaneoAsync();

    // ESTA ES LA CLAVE: Se llama en el loop para verificar si terminó
    // Devuelve: -1 (Escaneando), 0 (No encontró nada), >0 (Cantidad encontrada)
    int loop(); 

    int  getNetworkCount() const { return _networkCount; }
    String getSsid(int index) const;
    int    getRssi(int index) const;
    wifi_auth_mode_t getEncryption(int index) const;

    void   setSelectedIndex(int index);
    int    getSelectedIndex() const { return _selectedIndex; }
    String getSelectedSsid() const;

    void   setPassword(const String &password) { _password = password; }
    String getPassword() const { return _password; }

    // Info del sistema
    String get_AIoT_SSID(void);
    String get_AIoT_PWD(void);
    String get_AIoT_IP(void);
    String get_AIoT_DNS(void);
    String get_AIoT_MAC(void);

private:
    String _ssids[MAX_NETWORKS];
    int    _rssis[MAX_NETWORKS];
    wifi_auth_mode_t _encryptions[MAX_NETWORKS];

    int    _networkCount;
    int    _selectedIndex;
    String _password;
    
    // Variables nuevas para control asíncrono
    bool _isScanning; 
    unsigned long _lastScanCheck;
};

extern ScanWiFi_AIoT ScanWiFi; // Declaración externa para usarla en main

#endif // __SCANWIFI_AIOT_H__