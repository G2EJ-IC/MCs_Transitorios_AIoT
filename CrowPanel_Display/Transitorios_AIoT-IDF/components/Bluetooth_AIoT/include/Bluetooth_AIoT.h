#ifndef BLUETOOTH_AIOT_H
#define BLUETOOTH_AIOT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Inicializa la pila Bluetooth (llamado al arranque)
void Bluetooth_AIoT_Init(void);

// Activa la publicidad (Advertising) para que el celular lo vea
void Bluetooth_Start_Advertising(void);

// Detiene el Bluetooth para ahorrar energía
void Bluetooth_Stop(void);

// Retorna true si hay un dispositivo conectado actualmente
bool Bluetooth_Is_Connected(void);

#ifdef __cplusplus
}
#endif

#endif // BLUETOOTH_AIOT_H