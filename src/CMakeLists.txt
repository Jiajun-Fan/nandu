cmake_minimum_required (VERSION 3.7)
project (queue)

file (GLOB SERVER_SRC "boss/*.cc")
file (GLOB CLIENT_SRC "worker/*.cc")
#file (GLOB TEST_SRC "test/main.cc")
file (GLOB COMMON_SRC "*.cc")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-psabi")
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
set (LIB_OPENSSL /usr/lib/arm-linux-gnueabihf/libcrypto.a)
set (CMAKE_EXE_LINKER_FLAGS "-pthread -lrt ${CMAKE_EXE_LINKER_FLAGS}")

add_executable (nandu_boss ${COMMON_SRC} ${SERVER_SRC})
target_link_libraries(nandu_boss ${LIB_OPENSSL})

add_executable (nandu_worker ${COMMON_SRC} ${CLIENT_SRC})
target_link_libraries(nandu_worker ${LIB_OPENSSL})

#add_executable (nandu_test ${COMMON_SRC} ${TEST_SRC})
#target_link_libraries(nandu_test ${LIB_OPENSSL})
