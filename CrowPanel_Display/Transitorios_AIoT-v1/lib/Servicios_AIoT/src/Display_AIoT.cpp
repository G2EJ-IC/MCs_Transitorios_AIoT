#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include <SPI.h>
#include "GFX_Class_Conf.h" // Aquí dentro ya se crea 'tft', por eso borramos la línea abajo

#include "Display_AIoT.h"
#include "IO_AIoT.h"
#include "TP_AIoT.h"

#include "esp_freertos_hooks.h"
#include "ui.h"

extern tp_service tp;       
extern io_service io_tp;    
extern SemaphoreHandle_t lvgl_mutex; 

// NOTA: Borramos 'LGFX tft;' de aquí porque ya está en tu .h

display_service::display_service() {}
display_service::~display_service() {}

/*Configuraciones de Buffer*/
static const uint16_t N = 8u; 
enum { SCREENBUFFER_SIZE_PIXELS = (TFT_WIDTH * TFT_HEIGHT / N) * (LV_COLOR_DEPTH / 8) };
static lv_color_t buf [SCREENBUFFER_SIZE_PIXELS];

// Función Flush (Volcar a pantalla)
void display_service::my_disp_flush (lv_display_t *disp, const lv_area_t *area, uint8_t *pixelmap)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    // Usamos la variable 'tft' que viene del include GFX_Class_Conf.h
    if (lv_disp_flush_is_last(disp)) {
        tft.pushImageDMA( area->x1, area->y1, w, h, (lgfx::rgb565_t*) pixelmap );
    } else {
        tft.pushImageDMA( area->x1, area->y1, w, h, (lgfx::rgb565_t*) pixelmap );
    }

    lv_disp_flush_ready( disp );
}

// Función Leer Táctil
void display_service::my_touchpad_read (lv_indev_t * indev_driver, lv_indev_data_t * data)
{
    uint16_t touchX = 0, touchY = 0;
    bool touched = tft.getTouch(&touchX, &touchY);

    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void display_service::setup()
{
    tft.init();
    tft.setBrightness(255);
    
    lv_init();

    // Configuración del driver de pantalla LVGL 9
    lv_display_t * disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Configuración del driver táctil
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    ui_init();
}

void display_service::loop()
{
    // 1. GESTIÓN DEL TIEMPO (ESTO HACE QUE LA PANTALLA "CAMINE")
    static uint32_t last_tick = 0;
    uint32_t now = millis();

    ui_tick();
    
    if (last_tick == 0) last_tick = now;
    
    uint32_t tick_diff = now - last_tick;
    if (tick_diff > 0) {
        lv_tick_inc(tick_diff); // <--- ¡VITAL! Sin esto la pantalla se congela al cambiar
        last_tick = now;
    }

    // 2. DIBUJO SEGURO CON SEMÁFORO
    if (lvgl_mutex != NULL) {
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            lv_timer_handler(); 
            xSemaphoreGive(lvgl_mutex);
        }
    } else {
        lv_timer_handler();
    }
}
