#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Inicializa el PWM para el brillo y los timers
void sys_control_aiot_init(void);

// Controlar el brillo (0-100)
void sys_control_aiot_set_backlight(int32_t percent);

// Obtener el tiempo formateado
void sys_control_aiot_get_uptime(char *buffer, size_t size);

// --- NUEVAS FUNCIONES PARA SUSPENSIÓN ---

// Configura el tiempo de espera según el índice del DropDown (0=15s, 1=30s...)
void sys_control_aiot_set_sleep_index(int index);

// Función maestra: llamar en el loop principal. Revisa si toca dormir o despertar.
void sys_control_aiot_manage_sleep(void);

#ifdef __cplusplus
}
#endif