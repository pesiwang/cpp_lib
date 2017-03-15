CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(async_client)

SET(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -DDEBUG")

INCLUDE_DIRECTORIES(.)

SET(ASYNC_SRC_LIST helper.cpp buffer.cpp async_client.cpp test_async_client.cpp)
ADD_EXECUTABLE(test_async_client.exe ${ASYNC_SRC_LIST})
TARGET_LINK_LIBRARIES(test_async_client.exe)


SET(MAP_SRC_LIST helper.cpp buffer.cpp async_client.cpp kv_map_client.cpp test_map_client.cpp)
AUX_SOURCE_DIRECTORY(map_server_def MAP_SRC_LIST)
ADD_EXECUTABLE(test_map_client.exe ${MAP_SRC_LIST})
TARGET_LINK_LIBRARIES(test_map_client.exe protobuf)



