idf_component_register(
    SRCS "main.cpp"
    
    # LISTA DE DEPENDENCIAS OBLIGATORIA
    REQUIRES nvs_flash 
             esp_timer 
             driver 
             esp_wifi 
             lvgl 
             ui 
             wifi_manager
             bsp_display
             io_services
)
