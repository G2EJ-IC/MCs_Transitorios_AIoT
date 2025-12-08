# Buscar archivos en la raíz del componente (.)
file(GLOB SRCS "*.c" "*.cpp")

idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "."
    REQUIRES lvgl
)