add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PUBLIC 
    -Wall
    -Werror
    $<$<CONFIG:Debug>:-g -O0>
    #-ffile-prefix-map=${CMAKE_SOURCE_DIR}=server
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

#edit necessary c files needed to build application. No testing or header files.
target_sources(${PROJECT_NAME} PRIVATE main.c server.c)