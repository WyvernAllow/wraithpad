cmake_minimum_required(VERSION 3.5.0)
project("wraithpad" VERSION 0.1.0 LANGUAGES C)

add_executable(${PROJECT_NAME}
    "src/main.c"
    "src/platform.h"
    "src/platform_unix.c"
)