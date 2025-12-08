#include "IO_AIoT.h"
#include "esp_log.h"

// Nota: No incluimos bsp_display.h aquí para evitar tentaciones de apagar la pantalla

static const char *TAG = "IO_SERVICE";

io_service::io_service() {}
io_service::~io_service() {}

void io_service::setup() {
    // VACIADO: No tocamos el LEDC (PWM).
    // El control del Backlight es propiedad exclusiva de bsp_display.c ahora.
    ESP_LOGW(TAG, "IO Setup neutralizado para proteger Backlight.");
}

void io_service::loop() {
    // Nada que hacer aquí por ahora
}

void io_service::setOpacity(uint32_t val) {
    // VACIADO: Ignoramos órdenes de cambiar brillo.
    // ESP_LOGI(TAG, "Ignorando setOpacity(%lu)", val);
}

void io_service::setPinBL(bool state) {
    // VACIADO: Ignoramos órdenes de apagar pantalla.
    // ESP_LOGI(TAG, "Ignorando setPinBL(%d)", state);
}

uint32_t io_service::suspender(void) {
    // Retornamos un valor alto para que nunca crea que debe dormir
    return 999999; 
}
