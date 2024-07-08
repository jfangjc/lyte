file(GLOB FILES "*.c" "*.h")
target_sources(${PROJECT_NAME} PRIVATE ${FILES})

add_subdirectory(compiler)