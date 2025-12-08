idf_component_register(
    SRCS "main.cpp"
	
	INCLUDE_DIRS "."
	
    # LISTA DE DEPENDENCIAS OBLIGATORIA
    # REQUIRES nvs_flash 
    #         esp_timer 
    #         driver 
    #         esp_wifi 
    #         bsp_display
)