#include "mSIM_AIoT.h"

// Ajustar si usas otro puerto serial, pero el 1 o 2 son estándar en ESP32
HardwareSerial SerialAT(1); 

// Instancia Global (Inicializada vacía, se configura en el constructor)
mSIM_AIoT_Class mSIM_AIoT(16, 17, "internet.comcel.com.co"); // Pines por defecto y APN Claro

mSIM_AIoT_Class::mSIM_AIoT_Class(uint8_t rx, uint8_t tx, const char* apn) {
    _rxPin = rx;
    _txPin = tx;
    _apn = apn;
    _user = ""; // Usuario vacío usualmente
    _pass = ""; // Clave vacía usualmente
    _connected = false;
    _lastCheck = 0;
}

void mSIM_AIoT_Class::begin(uint32_t baud) {
    // 1. Iniciar Serial
    SerialAT.begin(baud, SERIAL_8N1, _rxPin, _txPin);
    
    // 2. Vincular TinyGSM
    _modem = new TinyGsm(SerialAT);
    _client = new TinyGsmClient(*_modem);

    Serial.println("mSIM_AIoT: Reiniciando Modem...");
    
    // Intentar reiniciar y establecer comunicación AT
    if (!_modem->restart()) {
        Serial.println("mSIM_AIoT: Error.. No responde el modulo.");
        return;
    }
    
    String info = _modem->getModemInfo();
    Serial.println("mSIM_AIoT: Modulo Info: " + info);
    
    // Esperar red y conectar GPRS inicial
    loop(); 
}

// --- EL CEREBRO TIPO WIFI ---
void mSIM_AIoT_Class::loop() {
    // Solo verificamos estado cada 5 segundos para no bloquear el sistema
    if (millis() - _lastCheck < 5000) return;
    _lastCheck = millis();

    // 1. Verificar Red Física (Torre)
    if (!_modem->isNetworkConnected()) {
        Serial.println("mSIM_AIoT: Buscando Red Celular...");
        if (!_modem->waitForNetwork(10000L)) { // Esperar max 10s
            Serial.println("mSIM_AIoT: ... Sin señal.");
            _connected = false;
            return;
        }
        Serial.println("mSIM_AIoT: Torre conectada.");
    }

    // 2. Verificar Datos (Internet)
    if (!_modem->isGprsConnected()) {
        Serial.println("mSIM_AIoT: Conectando a Internet (APN)...");
        if (!_modem->gprsConnect(_apn, _user, _pass)) {
            Serial.println("mSIM_AIoT: ... Fallo conexion datos.");
            _connected = false;
            return;
        }
        Serial.println("mSIM_AIoT: INTERNET CONECTADO.");
    }

    _connected = true;
}

bool mSIM_AIoT_Class::isConnected() {
    return _connected; // Retorna rápido sin bloquear
}

// --- GPS ---
bool mSIM_AIoT_Class::enableGPS() {
    return _modem->enableGPS();
}

bool mSIM_AIoT_Class::disableGPS() {
    return _modem->disableGPS();
}

bool mSIM_AIoT_Class::getGPS(float &lat, float &lon, float &speed, float &alt, int &vsat, int &usat) {
    // vsat = satelites visibles, usat = usados
    float accuracy;
    int year, month, day, hour, min, sec;
    return _modem->getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy, &year, &month, &day, &hour, &min, &sec);
}

// --- INFO ---
String mSIM_AIoT_Class::getIP() {
    return _modem->getLocalIP();
}

String mSIM_AIoT_Class::getOperator() {
    return _modem->getOperator();
}

String mSIM_AIoT_Class::getSignalQuality() {
    return String(_modem->getSignalQuality());
}

TinyGsmClient& mSIM_AIoT_Class::getClient() {
    return *_client;
}