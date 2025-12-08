# CMakeLists.txt para el componente 'ui' generado por EEZ Studio
# Objetivo: Compilar automáticamente cualquier estructura de archivos generada.

# 1. Registrar el componente y sus dependencias
# REQUIRES lvgl: Es CRÍTICO que sea público (REQUIRES) y no privado,
# porque ui.h incluye cabeceras de LVGL. Si no se hace así, main.c fallará al compilar.
idf_component_register(
    REQUIRES lvgl
)

# 2. Definir rutas base
set(UI_SRC_DIR "${COMPONENT_DIR}/src")

# 3. Recolección automática de fuentes (.c) y cabeceras (.h)
# La función file(GLOB_RECURSE...) busca en src/ y todos sus subdirectorios.
file(GLOB_RECURSE UI_SOURCES "${UI_SRC_DIR}/*.c")
file(GLOB_RECURSE UI_HEADERS "${UI_SRC_DIR}/*.h")

# 4. Inyección de fuentes al target de la librería del componente
target_sources(${COMPONENT_LIB} PRIVATE ${UI_SOURCES})

# 5. Gestión de Directorios de Inclusión (Include Paths)
# EEZ Studio a veces genera includes relativos y a veces planos.
# Para robustez, añadimos todos los subdirectorios que contengan.h al path de inclusión.
set(UI_INCLUDE_DIRS "${UI_SRC_DIR}")
foreach(HEADER_FILE ${UI_HEADERS})
    get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
    list(APPEND UI_INCLUDE_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES UI_INCLUDE_DIRS)

# Aplicar los directorios de inclusión públicamente
target_include_directories(${COMPONENT_LIB} PUBLIC ${UI_INCLUDE_DIRS})

# 6. Supresión de Advertencias (Warnings)
# El código generado automáticamente a menudo contiene variables no usadas.
# Limpiamos la salida del compilador suprimiendo estos warnings específicos en este componente.
target_compile_options(${COMPONENT_LIB} PRIVATE 
    -Wno-unused-variable 
    -Wno-unused-function
    -Wno-unused-but-set-variable
)

message(STATUS "EEZ Studio UI Component: Sources detected and configured.")
