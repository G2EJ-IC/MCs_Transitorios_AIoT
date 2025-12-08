idf_component_register(
    SRCS "src/display_hal.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd lvgl driver esp_timer esp_lcd_touch_gt911
)