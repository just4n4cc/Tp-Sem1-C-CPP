cmake_minimum_required(VERSION 3.1)

project(TryCI VERSION 1.0
        LANGUAGES C CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

##
### Code coverage ###
##
if (CMAKE_BUILD_TYPE MATCHES Debug)
  include(CodeCoverage)
  append_coverage_compiler_flags()
  setup_target_for_coverage_gcovr_html(
    NAME coverage
    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
    EXCLUDE "build/*" "gtests/*" "src/main.c")
endif()

##
### Source definitions ###
##
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ggdb3")

set(LIB_SOURCE_FILES src/funcs.c)
include_directories(include)

add_library(orders_manage_lib STATIC ${LIB_SOURCE_FILES})
add_executable(prog src/main.c)
target_link_libraries(prog orders_manage_lib)

##
### Test definitions ###
##
enable_testing()
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
message(${GTEST_LIBRARIES})

add_executable(tests gtests/main.cpp gtests/tests.cpp)

target_link_libraries(tests ${GTEST_LIBRARIES} -lpthread)
target_link_libraries(tests orders_manage_lib)
