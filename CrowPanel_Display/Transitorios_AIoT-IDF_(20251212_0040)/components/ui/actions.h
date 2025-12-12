#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_fn_connec(lv_event_t * e);
extern void action_fn_re_scan(lv_event_t * e);
extern void action_fn_connec_aio_t(lv_event_t * e);
extern void action_fn_text_area_ssid(lv_event_t * e);
extern void action_fn_text_area_password(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/