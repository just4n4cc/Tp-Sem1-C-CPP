cmake_minimum_required(VERSION 3.8)
project(OrderListSplit)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ggdb3")

set(LIB_SOURCE_FILES src/funcs.c)
include_directories(include)

add_library(orders_manage_lib STATIC ${LIB_SOURCE_FILES})
add_executable(prog src/main.c)
target_link_libraries(prog orders_manage_lib)

enable_testing()
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
message(${GTEST_LIBRARIES})

add_executable(tests gtests/tests.cpp)

target_link_libraries(tests ${GTEST_LIBRARIES} -lpthread)
target_link_libraries(tests orders_manage_lib)
