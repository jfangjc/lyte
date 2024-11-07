cmake_minimum_required(VERSION 3.25)

project(lyte LANGUAGES C)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

set_property(TARGET lyte PROPERTY C_STANDARD 11)