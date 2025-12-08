# Buscar automáticamente todos los .c y .cpp
file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

idf_component_register(
    SRCS ${SOURCES} "ui.c"
					"images.c"
					"screens.c"
					"styles.c"
					"ui_image_aio_t.c"
					"ui_image_aio_t_235x157.c"
					"ui_image_ud_fjd_c.c"
					"ui_image_ud_fjd_c_058x077.c" 
    INCLUDE_DIRS "."
    
    # La UI depende de LVGL y de tu WiFi Manager
    REQUIRES lvgl wifi_manager
)
