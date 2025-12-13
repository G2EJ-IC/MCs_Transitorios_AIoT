#include "vars.h"
#include <string>
#include <cstring>

// -----------------------------------------------------------------------------
// 1. STATE STORAGE (Static Private Variables)
// -----------------------------------------------------------------------------

// --- Integers and Booleans ---
static int32_t g_slider_porcentaje = 100; // Default 100%
static bool g_connec = false;
static bool g_re_scan = false;

// --- Dropdowns (Indices) ---
static int32_t g_drop_down_suspender = 0;
static int32_t g_drop_down_metodo = 0;

// --- Input Texts (User Input) ---
static std::string g_text_area_ssid_value = "";
static std::string g_text_area_pass_value = "";

// --- Output Texts (Status Labels) ---
static std::string g_ui_lab_ssid = "Disconnected";
static std::string g_ui_lab_ip   = "0.0.0.0";
static std::string g_ui_lab_dns  = "0.0.0.0";
static std::string g_ui_lab_mac  = "00:00:00:00:00:00";

// --- Clock Texts (DHMS) ---
static std::string g_label_dhms_1 = "";
static std::string g_label_dhms_2 = "";

// -----------------------------------------------------------------------------
// 2. IMPLEMENTATION (Bridge C <-> C++)
// -----------------------------------------------------------------------------
extern "C" {

// --- Slider Percentage ---
int32_t get_var_slider_porcentaje() { return g_slider_porcentaje; }
void set_var_slider_porcentaje(int32_t value) { g_slider_porcentaje = value; }

// --- Boolean Connection Status ---
bool get_var_connec() { return g_connec; }
void set_var_connec(bool value) { g_connec = value; }

// --- Boolean ReScan Trigger ---
bool get_var_re_scan() { return g_re_scan; }
void set_var_re_scan(bool value) { g_re_scan = value; }

// --- Dropdown Suspension ---
int32_t get_var_drop_down_suspender() { return g_drop_down_suspender; }
void set_var_drop_down_suspender(int32_t value) { g_drop_down_suspender = value; }

// --- Dropdown Method ---
int32_t get_var_drop_down_metodo() { return g_drop_down_metodo; }
void set_var_drop_down_metodo(int32_t value) { g_drop_down_metodo = value; }

// --- Text Area SSID (Input) ---
const char *get_var_text_area_ssid_value() { return g_text_area_ssid_value.c_str(); }
void set_var_text_area_ssid_value(const char *value) { g_text_area_ssid_value = value; }

// --- Text Area Password (Input) ---
const char *get_var_text_area_pass_value() { return g_text_area_pass_value.c_str(); }
void set_var_text_area_pass_value(const char *value) { g_text_area_pass_value = value; }

// --- Label SSID (Output) ---
const char *get_var_ui_lab_ssid() { return g_ui_lab_ssid.c_str(); }
void set_var_ui_lab_ssid(const char *value) { g_ui_lab_ssid = value; }

// --- Label IP (Output) ---
const char *get_var_ui_lab_ip() { return g_ui_lab_ip.c_str(); }
void set_var_ui_lab_ip(const char *value) { g_ui_lab_ip = value; }

// --- Label DNS (Output) ---
const char *get_var_ui_lab_dns() { return g_ui_lab_dns.c_str(); }
void set_var_ui_lab_dns(const char *value) { g_ui_lab_dns = value; }

// --- Label MAC (Output) ---
const char *get_var_ui_lab_mac() { return g_ui_lab_mac.c_str(); }
void set_var_ui_lab_mac(const char *value) { g_ui_lab_mac = value; }

// --- Label DHMS 1 (Clock Tab 1) ---
const char *get_var_label_dhms_1() { return g_label_dhms_1.c_str(); }
void set_var_label_dhms_1(const char *value) { g_label_dhms_1 = value; }

// --- Label DHMS 2 (Clock Tab 2) ---
const char *get_var_label_dhms_2() { return g_label_dhms_2.c_str(); }
void set_var_label_dhms_2(const char *value) { g_label_dhms_2 = value; }

} // End extern "C"