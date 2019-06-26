cmake_minimum_required (VERSION 3.7)
project (queue)

# build queue
file (GLOB SRC "*.cc")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
message (0 "${CMAKE_CXX_FLAGS}")

add_executable (nandu_queue ${SRC})
