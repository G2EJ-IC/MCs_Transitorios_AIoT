# components/ui/CMakeLists.txt

# Registrar fuentes recursivamente (Nota: Práctica aceptable para código generado)
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h")

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "src"
    REQUIRES lvgl Configuracion_AIoT
)
