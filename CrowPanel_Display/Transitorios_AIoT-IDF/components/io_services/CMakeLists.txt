idf_component_register(
    SRCS "src/Display_AIoT.cpp"
         "src/IO_AIoT.cpp"
    INCLUDE_DIRS "include"
    
    # Dependencias del sistema y gráficas
    REQUIRES driver 
             esp_timer 
             log 
             lvgl 
             ui
             bsp_display
)