file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

idf_component_register(
    SRC_DIRS "src" "ui"
    INCLUDE_DIRS "src" "ui"
    REQUIRES lvgl WiFi_AIoT System_Control_AIoT
)