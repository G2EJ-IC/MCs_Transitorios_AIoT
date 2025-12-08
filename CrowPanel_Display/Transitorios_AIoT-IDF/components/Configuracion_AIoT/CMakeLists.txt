# Este archivo permite la compilación automática de todos los recursos generados por EEZ Studio
idf_component_register(
    SRC_DIRS    "src" 
                "src/screens" 
                "src/images" 
                "src/fonts" 
                "src/actions" 
                "src/vars"
    INCLUDE_DIRS "src"
    REQUIRES lvgl
)
