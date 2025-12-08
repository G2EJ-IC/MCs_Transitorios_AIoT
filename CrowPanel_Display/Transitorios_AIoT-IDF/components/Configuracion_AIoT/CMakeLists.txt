idf_component_register(
    SRCS "src/Display_AIoT.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd driver freertos
)