cmake_minimum_required(VERSION 3.10)

project(hello_world VERSION 0.2)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(CONFIG_DIR "${PROJECT_SOURCE_DIR}/config")

aux_source_directory("${PROJECT_SOURCE_DIR}/src" SRC_LIST)

add_executable(hello_world ${SRC_LIST})

configure_file("${CONFIG_DIR}/hello_world_config.h.in" "${PROJECT_BINARY_DIR}/hello_world_config.h")

target_include_directories(hello_world PUBLIC
                            "${PROJECT_BINARY_DIR}"
                           )

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror"
)
