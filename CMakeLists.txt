cmake_minimum_required(VERSION 3.10)

project(esp_brookesia_simulator_vscode  LANGUAGES C CXX)

# Set definitions
set(SIMULATOR_DEF "-DSIMULATOR=1")                      # Flag for simulator
set(DISP_DEF "-DDISP_HOR_RES=1024 -DDISP_VER_RES=600")  # Resolution of the display
set(LVGL_DEF "-DLV_CONF_INCLUDE_SIMPLE")                # Used for LVGL related codes
set(LV_DRIVERS_DEF "-DUSE_SDL")                         # Used for lv_drivers related codes
set(ESP_BROOKESIA_DEF "-DESP_BROOKESIA_KCONFIG_IGNORE") # Used for ESP-Brookesia related codes
if(RUN_TEST)
set(SIMULATOR_DEF "${SIMULATOR_DEF} -DRUN_TEST=1")                      # Flag for simulator
endif()
# Set C/CXX toolchain
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
    message(STATUS "Detected system: Linux")
    message(STATUS "Using toolchain C: ${CMAKE_C_COMPILER}")
    message(STATUS "Using toolchain CXX: ${CMAKE_CXX_COMPILER}")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_C_COMPILER gcc.exe)
    set(CMAKE_CXX_COMPILER g++.exe)
    message(STATUS "Detected system: Windows")
    message(STATUS "Using toolchain C: ${CMAKE_C_COMPILER}")
    message(STATUS "Using toolchain CXX: ${CMAKE_CXX_COMPILER}")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
    message(STATUS "Detected system: MacOS")
    message(STATUS "Using toolchain C: ${CMAKE_C_COMPILER}")
    message(STATUS "Using toolchain CXX: ${CMAKE_CXX_COMPILER}")
endif()
# Set C/C++ standards
set(CMAKE_C_STANDARD 11)        # C11
set(CMAKE_CXX_STANDARD 17)      # C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set compiler flags
set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Wshadow -Wundef -Wno-unused-function")
set(CMAKE_CXX_FLAGS "-O0 -g -Wall -Wextra -Wshadow -Wundef -Wno-unused-function -Wno-unused-parameter \
                    -Wno-missing-field-initializers")
# Set cmake options
set(CMAKE_OBJECT_PATH_MAX 512)  # Avoid long path error in Windows
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
# Set componets directory
set(MAIN_DIR ${CMAKE_SOURCE_DIR}/main)
set(COMPONENTS_DIR ${CMAKE_SOURCE_DIR}/components)
set(LVGL_DIR ${COMPONENTS_DIR}/lvgl)
set(LV_DRIVERS_DIR ${COMPONENTS_DIR}/lv_drivers)
set(ESP_BROOKESIA_DIR ${COMPONENTS_DIR}/esp-brookesia/src)
set(ESP_BROOKESIA_APP_DIR ${COMPONENTS_DIR}/esp-brookesia-app)
set(ESP_BROOKESIA_STYLESHEET_DIR ${COMPONENTS_DIR}/esp-brookesia-stylesheet)

# Include componets directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${COMPONENTS_DIR}
    ${LVGL_DIR}
    ${LV_DRIVERS_DIR}
    ${ESP_BROOKESIA_DIR}
    ${ESP_BROOKESIA_APP_DIR}
    ${ESP_BROOKESIA_STYLESHEET_DIR}
)

# Search C/CXX sources
file(GLOB_RECURSE MAIN_SRCS "${MAIN_DIR}/*.c"  "${MAIN_DIR}/*.cpp") # Main sources
file(GLOB_RECURSE LVGL_SRCS "${LVGL_DIR}/src/*.c")                  # LVGL sources
file(GLOB_RECURSE LV_DRIVER_SRCS "${LV_DRIVERS_DIR}/*.c")           # lv_drivers sources
file(GLOB_RECURSE ESP_BROOKESIA_SRCS "${ESP_BROOKESIA_DIR}/*.c" "${ESP_BROOKESIA_DIR}/*.cpp")
                                                                    # ESP-Brookesia sources
file(GLOB_RECURSE ESP_BROOKESIA_APP_SRCS "${ESP_BROOKESIA_APP_DIR}/*.c" "${ESP_BROOKESIA_APP_DIR}/*.cpp")
                                                                    # ESP-Brookesia apps sources
file(GLOB_RECURSE ESP_BROOKESIA_STYLESHEET_SRCS                     # ESP-Brookesia stylesheets sources
    "${ESP_BROOKESIA_STYLESHEET_DIR}/*.c"
    "${ESP_BROOKESIA_STYLESHEET_DIR}/*.cpp"
)
# Set definitions for sources
set_source_files_properties(${MAIN_SRCS}                            # Main definitions
    PROPERTIES COMPILE_FLAGS "${SIMULATOR_DEF} ${DISP_DEF} ${LVGL_DEF} ${LV_DRIVERS_DEF} ${ESP_BROOKESIA_DEF}"
)
set_source_files_properties(${LVGL_SRCS} PROPERTIES COMPILE_FLAGS "${LVGL_DEF}")
                                                                    # LVGL definitions
set_source_files_properties(${LV_DRIVER_SRCS} PROPERTIES COMPILE_FLAGS "${LV_DRIVERS_DEF} ${DISP_DEF}")
                                                                    # lv_drivers definitions
set_source_files_properties(${ESP_BROOKESIA_SRCS} PROPERTIES COMPILE_FLAGS "${ESP_BROOKESIA_DEF}")
                                                                    # ESP-Brookesia definitions
set_source_files_properties(${ESP_BROOKESIA_APP_SRCS} PROPERTIES COMPILE_FLAGS "${SIMULATOR_DEF} ${ESP_BROOKESIA_DEF}")
                                                                    # ESP-Brookesia apps definitions
set_source_files_properties(${ESP_BROOKESIA_STYLESHEET_SRCS}        # ESP-Brookesia stylesheets definitions
    PROPERTIES COMPILE_FLAGS "${SIMULATOR_DEF} ${ESP_BROOKESIA_DEF}"
)
# Add C/CXX sources
add_executable(esp_brookesia_simulator_vscode
    ${MAIN_SRCS}
    ${LVGL_SRCS}
    ${LV_DRIVER_SRCS}
    ${ESP_BROOKESIA_SRCS}
    ${ESP_BROOKESIA_APP_SRCS}
    ${ESP_BROOKESIA_STYLESHEET_SRCS}
)

# Link SDL2 library
find_package(SDL2 REQUIRED)
target_link_libraries(esp_brookesia_simulator_vscode SDL2main)
target_link_libraries(esp_brookesia_simulator_vscode SDL2)

# Add target: clean_build (command: make clean_build)
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}
)
# Add target: run (command: make run)
add_custom_target (run
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/esp_brookesia_simulator_vscode
)
