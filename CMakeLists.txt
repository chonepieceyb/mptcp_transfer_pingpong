cmake_minimum_required(VERSION 3.10)

project(transfer_pingpong VERSION 0.1)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(CONFIG_DIR "${PROJECT_SOURCE_DIR}/config")

aux_source_directory("${PROJECT_SOURCE_DIR}/src" SRC_LIST)

add_executable(transfer_pingpong ${SRC_LIST})

configure_file("${CONFIG_DIR}/transfer_pingpong_config.h.in" "${PROJECT_BINARY_DIR}/transfer_pingpong_config.h")

target_include_directories(transfer_pingpong PUBLIC
                            "${PROJECT_BINARY_DIR}"
                           )

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -O2"
)
