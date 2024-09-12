cmake_minimum_required(VERSION 3.10)

project(esp_ui_simulator_vscode  LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Wshadow -Wundef -Wno-unused-function")
set(CMAKE_CXX_FLAGS "-O0 -g -Wall -Wextra -Wshadow -Wundef -Wno-unused-function")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE C_SOURCES "${CMAKE_SOURCE_DIR}/components/esp-ui/*.c" 
"${CMAKE_SOURCE_DIR}/components/esp-ui-app/*.c"
"${CMAKE_SOURCE_DIR}/components/lv_drivers/*.c"
"${CMAKE_SOURCE_DIR}/components/esp-ui-stylesheet/*.c"
"${CMAKE_SOURCE_DIR}/components/lvgl/demos/*.c" 
"${CMAKE_SOURCE_DIR}/components/lvgl/*.c")

file(GLOB_RECURSE CPP_SOURCES "${CMAKE_SOURCE_DIR}/components/esp-ui/src/*.cpp" 
"${CMAKE_SOURCE_DIR}/components/esp-ui-app/*.cpp"
"${CMAKE_SOURCE_DIR}/components/esp-ui-stylesheet/*.cpp"
)

# 包含目录
find_package(SDL2 REQUIRED SDL2)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/components/esp-ui
    ${CMAKE_SOURCE_DIR}/components/esp-ui/src
    ${CMAKE_SOURCE_DIR}/components/esp-ui-app/apps
    ${CMAKE_SOURCE_DIR}/components/esp-ui-app
    ${CMAKE_SOURCE_DIR}/components/lv_drivers
    ${CMAKE_SOURCE_DIR}/components/esp-ui-stylesheet
    ${CMAKE_SOURCE_DIR}/components/lvgl  # 确保 lvgl.h 在此路径下
    ${CMAKE_SOURCE_DIR}/components
)



# 设置定义和链接库
add_definitions(-DSIMULATOR=1 -DLV_BUILD_TEST=0 -DESP_UI_KCONFIG_IGNORE)

add_executable(esp_ui_simulator_vscode main/main.cpp main/mouse_cursor_icon.c ${C_SOURCES} ${CPP_SOURCES} )
add_compile_definitions(LV_CONF_INCLUDE_SIMPLE)
add_compile_definitions(USE_SDL)
add_compile_definitions(ESP_UI_KCONFIG_IGNORE)

target_link_libraries(esp_ui_simulator_vscode mingw32)  # new add
target_link_libraries(esp_ui_simulator_vscode SDL2main)  # new add
target_link_libraries(esp_ui_simulator_vscode SDL2)  # new add

# Clean target (类似于 Makefile 中的 `clean` 规则)
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
)
add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/esp_ui_simulator_vscode
)


