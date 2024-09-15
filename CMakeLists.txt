cmake_minimum_required(VERSION 3.5.0)
project("wraithpad" VERSION 0.1.0 LANGUAGES C)

if(UNIX)
    add_definitions(-DPLATFORM_UNIX)
elseif(WIN32)
    add_definitions(-DPLATFORM_WIN32)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_executable(${PROJECT_NAME}
    "src/main.c"
    "src/platform.h"
    "src/keys.h"
    "src/keys.c"
)