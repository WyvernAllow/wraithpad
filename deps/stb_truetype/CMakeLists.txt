cmake_minimum_required(VERSION 3.5)
project(stb_truetype)

add_library(${PROJECT_NAME}
    include/stb_truetype/stb_truetype.h
    src/stb_truetype.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include/
)