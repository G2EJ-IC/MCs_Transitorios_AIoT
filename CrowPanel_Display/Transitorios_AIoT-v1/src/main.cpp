#include "Arduino.h"

#include <config.h>
#include <Display_AIoT.h>
#include <TP_AIoT.h>
#include <IO_AIoT.h>

#include <ScanWiFi_AIoT.h>

#include "screens.h"

#include "vars.h"

TaskHandle_t Task1 = NULL;
TaskHandle_t Task2 = NULL;
TaskHandle_t Task3 = NULL;
TaskHandle_t Task4 = NULL;

SemaphoreHandle_t lvgl_mutex = NULL;

io_service io;            // load IO control service
display_service display;  // load display service
tp_service tp;            // load touchpad

uint32_t lv_N_sleep;

//************************************************************************************************
inline void loop_Task1(void);
inline void loop_Task2(void);
inline void loop_Task3(void);
inline void loop_Task4(void);
//************************************************************************************************
void actualizar_lista_wifi_en_UI();
//************************************************************************************************
void loop1(void *);
void loop2(void *);
void loop3(void *);
void loop4(void *);
//************************************************************************************************

// static uint32_t lvgl_refresh_timestamp = 5u;

void setup()
{
  // pinMode(PIN_BL, OUTPUT);
  // Segundo Siclo en el Núcleo Secundario.
  // Núcleo Principal  -> 1. APP
  // Núcleo Secundario -> 0. PRO
  /************************************Begin FreeRTOS*******************************************/
  
  Serial.begin(115200);

    // Crear el mutex para LVGL
  lvgl_mutex = xSemaphoreCreateMutex();

  BaseType_t taskCreationResult;

  taskCreationResult = xTaskCreatePinnedToCore(
      loop1,
      "Task_1",
      11264,
      NULL,
      2,
      &Task1,
      0);
  if (taskCreationResult != pdPASS)
  {
    Serial.println("Error al crear Task1");
    while (true)
      ;
  }

  taskCreationResult = xTaskCreatePinnedToCore(
      loop2,
      "Task_2",
      63488, // 31744,
      NULL,
      3,
      &Task2,
      0);
  if (taskCreationResult != pdPASS)
  {
    Serial.println("Error al crear Task2");
    while (true)
      ;
  }

  taskCreationResult = xTaskCreatePinnedToCore(
      loop3,
      "Task_3",
      25600, //25600
      NULL,
      1,
      &Task3,
      0);
  if (taskCreationResult != pdPASS)
  {
    Serial.println("Error al crear Task3");
    while (true)
      ;
  }

  taskCreationResult = xTaskCreatePinnedToCore(
      loop4,
      "Task_4",
      25600, // 3200, //25600
      NULL,
      1,
      &Task4,
      0);
  if (taskCreationResult != pdPASS)
  {
    Serial.println("Error al crear Task4");
    while (true)
      ;
  }

  /******************************************End FreeRTOS***************************************/
  vTaskDelay(pdMS_TO_TICKS(1000)); // delay( 1000 );
  // lv_task_handler();
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(2)); // delay( 2 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************

inline void loop_Task1(void)
{
  io.loop();

  // VERIFICAR ESTADO DEL WIFI
  int resultado = ScanWiFi.loop(); 

  // Como ahora loop() devuelve -1 si está inactivo, esto solo entra UNA VEZ
  // justo cuando el escaneo termina.
  if (resultado >= 0) {
      Serial.println("Evento de escaneo detectado. Actualizando UI...");
      actualizar_lista_wifi_en_UI();
  }
}

inline void loop_Task2(void)
{
  display.loop();
  lv_N_sleep = io.suspender();
}

inline void loop_Task3(void)
{
  tp.loop();
}

inline void loop_Task4(void)
{
  tp.lv_no_sleep(lv_N_sleep);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************

void loop1(void *parameter)
{
  io.setup();
  ScanWiFi.begin(); 

  for (;;)
  {
    loop_Task1();
    vTaskDelay(pdMS_TO_TICKS(5)); 
  }
}

void loop2(void *parameter)
{
  int delayLength2 = 200;
  unsigned long asyncDelay2 = 0;

  display.setup();
  for (;;)
  {
    loop_Task2();
    tp.lv_no_sleep(lv_N_sleep);
    if (millis() >= asyncDelay2)
    {
      io.cronometro(asyncDelay2);
      asyncDelay2 += delayLength2;
    }
    vTaskDelay(pdMS_TO_TICKS(2)); // delay( 2 );
    // taskYIELD();
  }
}

void loop3(void *parameter)
{
  tp.setup();
  for (;;)
  {
    loop_Task3();
    vTaskDelay(pdMS_TO_TICKS(5)); // delay( 5 );
    // taskYIELD();
  }
}

void loop4(void *parameter)
{
  unsigned long asyncDelay4 = 0;
  tp.setup();
  for (;;)
  {
    loop_Task4();
    vTaskDelay(pdMS_TO_TICKS(5)); // delay( 5 );
    // taskYIELD();
  }
}

// Función auxiliar para actualizar la UI cuando llegan datos WiFi
void actualizar_lista_wifi_en_UI() {
  if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    int n = ScanWiFi.getNetworkCount();
    // Serial.printf("Actualizando UI con %d redes found\n", n);
    
    if (n == 0) {
        // Ejemplo: lv_dropdown_set_options(objects.dd_wifi, "No Networks Found");
        return;
    }

    // Construir una cadena con todas las redes separadas por saltos de línea (formato LVGL)
    String opciones = "";
    for (int i = 0; i < n; ++i) {
        opciones += ScanWiFi.getSsid(i);
        opciones += " (";
        opciones += String(ScanWiFi.getRssi(i));
        opciones += "dBm)\n"; // Salto de línea para la siguiente opción
    }

    // --- AQUÍ VA TU OBJETO DE EEZ STUDIO ---
    // Si usas un Roller:
    // lv_roller_set_options(objects.roller_wifi, opciones.c_str(), LV_ROLLER_MODE_NORMAL);
    
    // Si usas un Dropdown:
    // lv_dropdown_set_options(objects.dd_wifi, opciones.c_str());
    
    // Solo para debug por ahora si no recuerdas el nombre del objeto:
    Serial.println("Lista para UI generada:");
    Serial.println(opciones);

    xSemaphoreGive(lvgl_mutex);
  } else {
        Serial.println("Error: No pude tomar el semáforo para actualizar UI");
  }
}