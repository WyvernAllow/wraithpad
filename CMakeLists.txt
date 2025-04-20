cmake_minimum_required(VERSION 3.5)
project(wraithpad VERSION 0.1.0 LANGUAGES C)

add_executable(${PROJECT_NAME}
    src/main.c
    src/utils.c
    src/utils.h
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c99 -pedantic -Wall -Wextra)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/wp_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/wp_version.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/stb_truetype)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    glfw
    stb_truetype
)