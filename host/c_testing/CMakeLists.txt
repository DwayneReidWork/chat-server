enable_testing()
add_executable(${PROJECT_NAME}_tests)

target_include_directories(${PROJECT_NAME}_tests PRIVATE ../cmocka/include)
target_link_directories(${PROJECT_NAME}_tests PRIVATE ../cmocka/build/src)

target_compile_options(
    ${PROJECT_NAME}_tests
    PRIVATE
    -Wall
    -Werror
    -g
    -O0
    -ffile-prefix-map=${CMAKE_SOURCE_DIR}=./server
)

target_compile_definitions(${PROJECT_NAME}_tests
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)


target_include_directories(${PROJECT_NAME}_tests PRIVATE ./include)

target_link_libraries(
    ${PROJECT_NAME}_tests
    cmocka
)
#actual c code you need to test against with testing. [Files here should not contain main]

# target_sources(${PROJECT_NAME}_tests PRIVATE ../src/server.c)

target_sources(${PROJECT_NAME}_tests PRIVATE src/test_server.c)