cmake_minimum_required(VERSION 3.10)

project(transfer_pingpong VERSION 2.0)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(CONFIG_DIR "${PROJECT_SOURCE_DIR}/config")
set(EXT_DIR "${PROJECT_SOURCE_DIR}/ext")

#add your libaries 
#boost 
set(BOOST_DIR "${EXT_DIR}/boost_1_78_0")
find_library(BOOST_LIB NAMES boost_program_options HINTS "${BOOST_DIR}/lib" REQUIRED)

aux_source_directory("${PROJECT_SOURCE_DIR}/src" SRC_LIST)

#get all target files
set(TARGET_LIST "${PROJECT_SOURCE_DIR}/src/client.cxx"  "${PROJECT_SOURCE_DIR}/src/msg_client.cxx" "${PROJECT_SOURCE_DIR}/src/server.cxx" "${PROJECT_SOURCE_DIR}/src/ecfexp_client.cxx" "${PROJECT_SOURCE_DIR}/src/ecfexp_client2.cxx")
list(REMOVE_ITEM SRC_LIST ${TARGET_LIST})

message("${SRC_LIST}")

foreach(TF ${TARGET_LIST})
    get_filename_component(TN ${TF} NAME_WE)
    add_executable(${TN} ${SRC_LIST} ${TF})
    configure_file("${CONFIG_DIR}/${CMAKE_PROJECT_NAME}_config.h.in" "${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_config.h")
    target_include_directories(${TN} PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            "${BOOST_DIR}/include"
                           )
    target_link_libraries(${TN} ${BOOST_LIB})
endforeach()

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -O2"
)
