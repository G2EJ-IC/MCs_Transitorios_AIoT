idf_component_register(
    SRCS "main_AIoT.c"
    INCLUDE_DIRS "."
    REQUIRES Configuracion_AIoT EEZ_AIoT nvs_flash WiFi_AIoT System_Control_AIoT
)