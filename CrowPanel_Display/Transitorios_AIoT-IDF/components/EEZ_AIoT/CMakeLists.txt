idf_component_register(SRCS "ui/ui.c" 
                            "ui/screens.c" 
                            "ui/images.c" 
                            "src/actions.cpp"  
                            "src/vars.cpp"
                        INCLUDE_DIRS "include" "ui"
                        REQUIRES lvgl WiFi_AIoT IO_AIoT Bluetooth_AIoT)