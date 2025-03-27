cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lyte LANGUAGES C)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

set_property(TARGET lyte PROPERTY C_STANDARD 11)

