cmake_minimum_required(VERSION 3.5)
project(wraithpad VERSION 0.1.0 LANGUAGES C)

add_executable(${PROJECT_NAME}
    src/main.c
)

add_subdirectory(deps/glfw)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
)