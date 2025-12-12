# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# CRÍTICO: Forzar la búsqueda de componentes en la carpeta 'components'
set(EXTRA_COMPONENT_DIRS components)

# Incluir herramientas de compilación de ESP-IDF
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Nombre del Proyecto
project(CrowPanel_HMI)
