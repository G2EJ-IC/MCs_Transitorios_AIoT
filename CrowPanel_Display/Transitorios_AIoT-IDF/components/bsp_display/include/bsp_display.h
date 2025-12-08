#pragma once

#include "esp_err.h"
#include "lvgl.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa la pantalla y el touch
 * @return esp_err_t ESP_OK si todo sale bien
 */
esp_err_t bsp_display_init(void);

/**
 * @brief Obtener el objeto de pantalla de LVGL
 */
lv_display_t *bsp_get_lv_disp(void);

/**
 * @brief Obtener el objeto de entrada (touch) de LVGL
 */
lv_indev_t *bsp_get_lv_indev(void);

/**
 * @brief Controlar el brillo (0-100)
 */
void bsp_display_brightness(uint8_t percent);

/**
 * @brief Bloquear el mutex de LVGL (para thread safety)
 */
bool bsp_display_lock(int timeout_ms);

/**
 * @brief Desbloquear el mutex de LVGL
 */
void bsp_display_unlock(void);

#ifdef __cplusplus
}
#endif
