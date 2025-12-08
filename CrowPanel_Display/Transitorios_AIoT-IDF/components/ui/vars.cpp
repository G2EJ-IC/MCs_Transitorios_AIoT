#include <stdio.h>
#include <lvgl.h>

#include "vars.h"
#include "screens.h"
#include "structs.h"
#include "ui.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

int32_t selected_item_index = 100;

extern int32_t get_var_slider_porcentaje()
{
    return selected_item_index;    
}
extern void set_var_slider_porcentaje(int32_t value)
{
    selected_item_index = value;
}

bool connec;

extern "C" bool get_var_connec() {
    return connec;
}

extern "C" void set_var_connec(bool value) {
    connec = value;
}

bool re_scan;

extern "C" bool get_var_re_scan() {
    return re_scan;
}

extern "C" void set_var_re_scan(bool value) {
    re_scan = value;
}

std::string text_area_ssid_value;

extern "C" const char *get_var_text_area_ssid_value() {
    return text_area_ssid_value.c_str();
}

extern "C" void set_var_text_area_ssid_value(const char *value) {
    text_area_ssid_value = value;
}

#ifdef __cplusplus
}
#endif