CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(sync_logger)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")

INCLUDE_DIRECTORIES(.)

SET(ASYNC_SRC_LIST sync_logger.cpp test_sync_logger.cpp)
ADD_EXECUTABLE(test_sync_logger.exe ${ASYNC_SRC_LIST})
TARGET_LINK_LIBRARIES(test_sync_logger.exe)


