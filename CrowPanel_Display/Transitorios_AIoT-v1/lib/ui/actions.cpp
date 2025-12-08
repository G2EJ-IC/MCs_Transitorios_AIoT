#include <Arduino.h>

/** * @brief : Función Similar a cout << de C++, Debe estar despues de #include <Arduino.h> para que funcione.
* @param : T.
* @return : obj.
*/
template<class T>
inline Print &operator <<(Print &obj, T arg){ 
    obj.print(arg); 
    return obj; 
}

#include "actions.h"
#include "screens.h"
#include "vars.h"

#include "ScanWiFi_AIoT.h"

extern ScanWiFi_AIoT ScanWiFi;

#ifdef __cplusplus
extern "C" {
#endif

int counter;

// String TextArea_SSID_Value = "";
// String TextArea_Password_Value = "";

// Texto neutro al iniciar o fallar
static void ui_reset_wifi_labels()
{
    lv_label_set_text(objects.ui_lab_ssid, "Nombre WiFi (SSID)");
    lv_label_set_text(objects.ui_lab_ip,   "xx.xx.xx.xx");
    lv_label_set_text(objects.ui_lab_dns,  "xx.xx.xx.xx");
    lv_label_set_text(objects.ui_lab_mac,  "xx:xx:xx:xx:xx:xx");
}

// Esta función se llamaba antes directamente, ahora la usaremos desde main.cpp si es necesario
// o puedes dejarla aquí si planeas llamarla desde otro lado.
static void ui_set_first_ssid()
{
    int count = ScanWiFi.getNetworkCount();
    if (count <= 0)
    {
        lv_textarea_set_text(objects.text_area_ssid, "No WiFi Found");
        set_var_text_area_ssid_value("No WiFi Found");
    }
    else
    {
        // Pone el primero en la lista
        ScanWiFi.setSelectedIndex(0);
        String ssid = ScanWiFi.getSelectedSsid();
        lv_textarea_set_text(objects.text_area_ssid, ssid.c_str());
        set_var_text_area_ssid_value(ssid.c_str());
    }
}

// BTN: Connec
// NOTA: He cambiado la lógica. Ahora este botón INICIA el escaneo.
// Los resultados aparecerán cuando termine (ver main.cpp).
void action_fn_connec(lv_event_t * e)
{
    LV_UNUSED(e);
    set_var_connec(false);

    // INICIO CORREGIDO: Usamos la función asíncrona
    bool inicio = ScanWiFi.iniciarEscaneoAsync();

    if (inicio)
    {
        // Solo indicamos que empezó. 
        Serial.println("Boton Connect presionado: Iniciando escaneo en segundo plano...");
        
        // Opcional: Poner un texto temporal en la UI
        lv_textarea_set_text(objects.text_area_ssid, "Escaneando...");
        
        // ---------------------------Monitor serial-----------------------------//
        Serial << "\r\n";
        Serial << "\r\n=========================================================";
        Serial << "\r\nUniversidad Distrital Francisco José de Caldas";
        Serial << "\r\nFacultad Tecnológica | Maestría en Ingeniería Civil";
        Serial << "\r\nMonitoreo AIoT | @2024";
        Serial << "\r\nErnesto Jose Guerrero Gonzalez, IC, MSc(C), Esp.";
        Serial << "\r\n=========================================================";
        Serial << "\r\n\n";
    }
    else
    {
        Serial.println("Escaneo ya en progreso, espere...");
    }
    
    // NOTA: El bloque 'else { ui_reset_wifi_labels()... }' ya no va aquí 
    // porque no sabemos el resultado todavía.
}

// BTN: Re-Scan
void action_fn_re_scan(lv_event_t * e)
{
    LV_UNUSED(e);

    // INICIO CORREGIDO
    bool inicio = ScanWiFi.iniciarEscaneoAsync();

    if (inicio) {
        Serial.println("Re-Scan solicitado...");
        // Puedes poner un indicador visual en EEZ Studio si quieres
        // Ejemplo: lv_label_set_text(objects.label_status, "Buscando...");
    } else {
        Serial.println("Sistema ocupado escaneando...");
    }
}

void action_fn_teclado_ready(lv_event_t * e)
{
    // Tu código original del teclado (sin cambios)
    LV_UNUSED(e);
}

#ifdef __cplusplus
}
#endif
