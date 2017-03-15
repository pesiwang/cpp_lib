CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(async_client)

SET(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -DDEBUG")

INCLUDE_DIRECTORIES(.)

SET(ASYNC_SRC_LIST helper.cpp buffer.cpp async_client.cpp test_async_client.cpp)
ADD_EXECUTABLE(test_async_client.exe ${ASYNC_SRC_LIST})
TARGET_LINK_LIBRARIES(test_async_client.exe)

