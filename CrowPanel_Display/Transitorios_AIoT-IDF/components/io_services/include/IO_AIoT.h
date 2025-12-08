#ifndef __IO_AIOT_H__
#define __IO_AIOT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

class io_service {
public:
    io_service();
    ~io_service();
    
    void setup();
    void loop(); // <--- AGREGADO: Esto faltaba para que compile
    void setOpacity(uint32_t val);
    void setPinBL(bool state);
    uint32_t suspender(void);
};

#ifdef __cplusplus
}
#endif

#endif