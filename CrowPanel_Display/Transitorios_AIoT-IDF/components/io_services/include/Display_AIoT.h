#ifndef __DISPLAY_AIOT_H__
#define __DISPLAY_AIOT_H__

#include "lvgl.h"

class display_service {
public:
    display_service();
    ~display_service();
    void setup();
    void loop();
};

#endif