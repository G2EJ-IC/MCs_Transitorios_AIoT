idf_component_register(
    SRCS "src/bsp_display.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd driver esp_lcd_touch_xpt2046 lvgl
)
