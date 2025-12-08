#ifndef __MSIM_AIOT_H__
#define __MSIM_AIOT_H__

#include <Arduino.h>

// Definimos el modelo de modem para TinyGSM
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>

class mSIM_AIoT_Class {
public:
    // Constructor: Recibe pines y el APN (Nombre del punto de acceso)
    mSIM_AIoT_Class(uint8_t rx, uint8_t tx, const char* apn);

    // Inicia el hardware y el puerto serial
    void begin(uint32_t baud = 115200);

    // ESTA ES LA CLAVE: Mantener conexión (Llamar siempre en el bucle)
    // Se encarga de reconectar si se cae la señal, igual que WiFi.
    void loop(); 

    // Estado de conexión
    bool isConnected();
    
    // Funciones GPS
    bool enableGPS();
    bool disableGPS();
    // Retorna true si hay datos válidos. Pasa las variables por referencia.
    bool getGPS(float &lat, float &lon, float &speed, float &alt, int &vsat, int &usat);

    // Getters de información
    String getIP();
    String getOperator();
    String getSignalQuality();

    // Acceso al cliente para MQTT o HTTP
    TinyGsmClient& getClient();

private:
    uint8_t _rxPin;
    uint8_t _txPin;
    const char* _apn;
    const char* _user;
    const char* _pass;
    
    bool _connected;
    unsigned long _lastCheck; // Para no saturar preguntando estado
    
    // Instancias internas de TinyGSM
    TinyGsm* _modem;
    TinyGsmClient* _client;
    HardwareSerial* _serialAT;
};

extern mSIM_AIoT_Class mSIM_AIoT;

#endif // __MSIM_AIOT_H__