idf_component_register(INCLUDE_DIRS "include")

target_link_directories(${COMPONENT_LIB}
                        INTERFACE
                        "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CONFIG_IDF_TARGET}")
target_link_libraries(${COMPONENT_LIB} INTERFACE esp_image_effects)
