cmake_minimum_required(VERSION 3.5.0)
project("wraithpad" VERSION 0.1.0 LANGUAGES C)

set(SOURCES
    "src/main.c"
    "src/platform.h"
    "src/keys.h"
    "src/keys.c"
)

if(UNIX)
    add_definitions(-DPLATFORM_UNIX)
    list(APPEND SOURCES "src/platform_unix.c")
elseif(WIN32)
    add_definitions(-DPLATFORM_WIN32)
    list(APPEND SOURCES "src/platform_win32.c")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})