#include "Display_AIoT.h"
#include "bsp_display.h"
#include "ui.h" 
#include "esp_log.h"

static const char *TAG = "DISPLAY";

display_service::display_service() {}
display_service::~display_service() {}

void display_service::setup() {
    ESP_LOGI(TAG, "Setup BSP...");
    bsp_display_init();
    
    ESP_LOGI(TAG, "Setup UI EEZ...");
    ui_init(); // <--- ¡SIN COMENTARIOS! OBLIGATORIO
}

void display_service::loop() {
    lv_timer_handler(); // Mueve los gráficos
    ui_tick();          // Mueve la lógica de flujo
}
