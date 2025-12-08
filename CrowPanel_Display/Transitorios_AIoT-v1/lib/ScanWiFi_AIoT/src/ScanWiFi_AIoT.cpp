#include "ScanWiFi_AIoT.h"
// #include <SD_AIoT.h> // Descomenta si ya integraste la SD, si no, déjalo así por ahora.

// INCLUIMOS LIBRERÍAS DE BAJO NIVEL PARA REPARAR LA NVS
#include <nvs_flash.h>
#include <nvs.h>

ScanWiFi_AIoT ScanWiFi;

ScanWiFi_AIoT::ScanWiFi_AIoT()
    :   _networkCount(0),
        _selectedIndex(-1),
        _password(""),
        _isScanning(false),
        _lastScanCheck(0)
{
    // Inicializar arrays
    for (int i = 0; i < MAX_NETWORKS; ++i) {
        _ssids[i] = "";
        _rssis[i] = 0;
        _encryptions[i] = WIFI_AUTH_OPEN;
    }
}

void ScanWiFi_AIoT::begin() {
    // -----------------------------------------------------------
    // BLOQUE DE AUTORREPARACIÓN DE NVS (CRÍTICO)
    // -----------------------------------------------------------
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Si la memoria está corrupta, la borramos forzosamente
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    // -----------------------------------------------------------

    // Configuración WiFi a prueba de fallos
    WiFi.persistent(false); // NO guardar nada en Flash nunca más
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, true); // Borrar credenciales previas de la RAM
    delay(100);
    
    // Si usas SD, aquí iría la lógica de lectura, pero primero probemos que no se reinicie.
}

bool ScanWiFi_AIoT::iniciarEscaneoAsync() {
    if (_isScanning) return false; 

    // IMPORTANTE: Verificar si el WiFi arrancó antes de escanear
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("Error: El hardware WiFi no ha iniciado correctamente.");
        return false;
    }

    // Escaneo asíncrono (true) y mostrando ocultas (true)
    WiFi.scanNetworks(true, true); 
    _isScanning = true;
    _networkCount = 0;
    return true;
}

int ScanWiFi_AIoT::loop() {
    if (!_isScanning) return -1; 

    int n = WiFi.scanComplete();

    if (n == -2) {
        // Falló el escaneo
        WiFi.scanDelete();
        _isScanning = false;
        return -1;
    }
    else if (n == -1) {
        return -1; // Sigue trabajando...
    }
    else {
        // Escaneo terminado con éxito
        _networkCount = n;
        if (_networkCount > MAX_NETWORKS) _networkCount = MAX_NETWORKS;

        for (int i = 0; i < _networkCount; ++i) {
            _ssids[i] = WiFi.SSID(i);
            _rssis[i] = WiFi.RSSI(i);
            _encryptions[i] = WiFi.encryptionType(i);
        }

        WiFi.scanDelete();
        _isScanning = false;
        _selectedIndex = -1;
        
        return _networkCount;
    }
}

// --- GETTERS (Iguales que antes) ---
String ScanWiFi_AIoT::getSsid(int index) const {
    if (index < 0 || index >= _networkCount) return "";
    return _ssids[index];
}
int ScanWiFi_AIoT::getRssi(int index) const {
    if (index < 0 || index >= _networkCount) return 0;
    return _rssis[index];
}
wifi_auth_mode_t ScanWiFi_AIoT::getEncryption(int index) const {
    if (index < 0 || index >= _networkCount) return WIFI_AUTH_OPEN;
    return _encryptions[index];
}
void ScanWiFi_AIoT::setSelectedIndex(int index) {
    if (index < 0 || index >= _networkCount) _selectedIndex = -1;
    else _selectedIndex = index;
}
String ScanWiFi_AIoT::getSelectedSsid() const {
    if (_selectedIndex < 0 || _selectedIndex >= _networkCount) return "";
    return _ssids[_selectedIndex];
}
String ScanWiFi_AIoT::get_AIoT_SSID(void) { return WiFi.SSID(); }
String ScanWiFi_AIoT::get_AIoT_PWD(void)  { return _password; }
String ScanWiFi_AIoT::get_AIoT_IP(void)   { return WiFi.localIP().toString(); }
String ScanWiFi_AIoT::get_AIoT_DNS(void)  { return WiFi.dnsIP().toString(); }
String ScanWiFi_AIoT::get_AIoT_MAC(void)  { return WiFi.macAddress(); }
