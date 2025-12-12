#include "vars.h"
#include <string>
#include <cstring>

// -----------------------------------------------------------------------------
// 1. ALMACENAMIENTO DE ESTADO (Variables Estáticas Privadas)
// -----------------------------------------------------------------------------

// --- Enteros y Booleanos ---
static int32_t g_slider_porcentaje = 100; // Default 100%
static bool g_connec = false;
static bool g_re_scan = false;

// --- Dropdowns (Índices) ---
static int32_t g_drop_down_suspender = 0;
static int32_t g_drop_down_metodo = 0;

// --- Textos de Entrada (Inputs del Usuario) ---
static std::string g_text_area_ssid_value = "";
static std::string g_text_area_pass_value = "";

// --- Textos de Salida (Labels de Estado) ---
static std::string g_ui_lab_ssid = "Desconectado";
static std::string g_ui_lab_ip   = "0.0.0.0";
static std::string g_ui_lab_dns  = "0.0.0.0";
static std::string g_ui_lab_mac  = "00:00:00:00:00:00";

// --- Textos de Reloj (DHMS) ---
static std::string g_label_dhms_1 = "";
static std::string g_label_dhms_2 = "";

// -----------------------------------------------------------------------------
// 2. IMPLEMENTACIÓN DE FUNCIONES (Bridge C <-> C++)
// -----------------------------------------------------------------------------
extern "C" {

// --- Slider Porcentaje ---
int32_t get_var_slider_porcentaje() { return g_slider_porcentaje; }
void set_var_slider_porcentaje(int32_t value) { g_slider_porcentaje = value; }

// --- Boolean Connec ---
bool get_var_connec() { return g_connec; }
void set_var_connec(bool value) { g_connec = value; }

// --- Boolean ReScan ---
bool get_var_re_scan() { return g_re_scan; }
void set_var_re_scan(bool value) { g_re_scan = value; }

// --- Dropdown Suspender ---
int32_t get_var_drop_down_suspender() { return g_drop_down_suspender; }
void set_var_drop_down_suspender(int32_t value) { g_drop_down_suspender = value; }

// --- Dropdown Metodo ---
int32_t get_var_drop_down_metodo() { return g_drop_down_metodo; }
void set_var_drop_down_metodo(int32_t value) { g_drop_down_metodo = value; }

// --- Text Area SSID (Entrada) ---
const char *get_var_text_area_ssid_value() { return g_text_area_ssid_value.c_str(); }
void set_var_text_area_ssid_value(const char *value) { g_text_area_ssid_value = value; }

// --- Text Area Password (Entrada) ---
const char *get_var_text_area_pass_value() { return g_text_area_pass_value.c_str(); }
void set_var_text_area_pass_value(const char *value) { g_text_area_pass_value = value; }

// --- Label SSID (Salida) ---
const char *get_var_ui_lab_ssid() { return g_ui_lab_ssid.c_str(); }
void set_var_ui_lab_ssid(const char *value) { g_ui_lab_ssid = value; }

// --- Label IP (Salida) ---
const char *get_var_ui_lab_ip() { return g_ui_lab_ip.c_str(); }
void set_var_ui_lab_ip(const char *value) { g_ui_lab_ip = value; }

// --- Label DNS (Salida) ---
const char *get_var_ui_lab_dns() { return g_ui_lab_dns.c_str(); }
void set_var_ui_lab_dns(const char *value) { g_ui_lab_dns = value; }

// --- Label MAC (Salida) ---
const char *get_var_ui_lab_mac() { return g_ui_lab_mac.c_str(); }
void set_var_ui_lab_mac(const char *value) { g_ui_lab_mac = value; }

// --- Label DHMS 1 (Reloj Tab 1) ---
const char *get_var_label_dhms_1() { return g_label_dhms_1.c_str(); }
void set_var_label_dhms_1(const char *value) { g_label_dhms_1 = value; }

// --- Label DHMS 2 (Reloj Tab 2) ---
const char *get_var_label_dhms_2() { return g_label_dhms_2.c_str(); }
void set_var_label_dhms_2(const char *value) { g_label_dhms_2 = value; }

} // End extern "C"