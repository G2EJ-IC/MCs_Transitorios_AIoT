idf_component_register(
    SRCS "main_AIoT.c"
    INCLUDE_DIRS "."
    # Agregamos nvs_flash a la lista
    REQUIRES Configuracion_AIoT ui nvs_flash
)