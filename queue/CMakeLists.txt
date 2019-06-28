cmake_minimum_required (VERSION 3.7)
project (queue)

file (GLOB SERVER_SRC "server/*.cc")
file (GLOB CLIENT_SRC "client/*.cc")
file (GLOB COMMON_SRC "*.cc")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-psabi")
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
set (LIB_OPENSSL /usr/lib/arm-linux-gnueabihf/libcrypto.a)
set (CMAKE_EXE_LINKER_FLAGS "-pthread -lrt ${CMAKE_EXE_LINKER_FLAGS}")

add_executable (nandu_server ${COMMON_SRC} ${SERVER_SRC})
target_link_libraries(nandu_server ${LIB_OPENSSL})

add_executable (nandu_client ${COMMON_SRC} ${CLIENT_SRC})
target_link_libraries(nandu_client ${LIB_OPENSSL})
