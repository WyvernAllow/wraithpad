cmake_minimum_required(VERSION 3.5.0)
project("wraithpad" VERSION 0.1.0 LANGUAGES C)

add_executable(${PROJECT_NAME}
    "src/main.c"
    "src/platform.h"
    "src/keys.h"
    "src/keys.c"
)

if(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_UNIX)
elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WIN32)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()