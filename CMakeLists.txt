cmake_minimum_required(VERSION 3.10)
project(lyte C)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(LYTE_VERSION_MAJOR 0)
set(LYTE_VERSION_MINOR 0)
set(LYTE_VERSION_PATCH 0)

set(LYTE_VERSION "${LYTE_VERSION_MAJOR}.${LYTE_VERSION_MINOR}.${LYTE_VERSION_PATCH}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# set(ARCH_FOLDER "llvm")
# if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
#     set(ARCH_FOLDER "llvm")
# elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
#     set(ARCH_FOLDER "aarch64")
# else()
#     message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
# endif()

# Set output directories to the build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wformat=2
        -Wcast-align
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
    )
endif()

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")

# file(GLOB_RECURSE ARCH_SRC_FILE "${CMAKE_SOURCE_DIR}/src/arch/${ARCH_FOLDER}/*.c")
# list(APPEND SRC_FILES ${ARCH_SRC_FILE})

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    # ${CMAKE_SOURCE_DIR}/src/arch/${ARCH_FOLDER}
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR})

# Unit Tests
file(GLOB_RECURSE TEST_FILES "${CMAKE_SOURCE_DIR}/test/*.c")
list(FILTER SRC_FILES EXCLUDE REGEX ".*compiler.c$")

add_executable(unit_test ${TEST_FILES} ${SRC_FILES})
target_include_directories(unit_test PRIVATE ${CMAKE_SOURCE_DIR}/src)

install(TARGETS unit_test DESTINATION ${CMAKE_BINARY_DIR})
