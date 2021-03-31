cmake_minimum_required(VERSION 3.1)

project(TryCI VERSION 1.0
        LANGUAGES C CXX)

find_library(PTHREAD_LIBRARY pthread)

#
### User flags ###
#
option(TESTS "" OFF)
option(COVERAGE "" OFF)

##
### Static libraries adding
#
add_library(SEARCH_SUBSTR_C STATIC lib/search_substr_c/search_substr_c.c)
add_library(FILES_AND_STRS_MANAGE STATIC lib/files_and_strs_manage/files_and_strs_manage.c)
include_directories(lib/search_substr_c)
include_directories(lib/files_and_strs_manage)
target_link_libraries(SEARCH_SUBSTR_C FILES_AND_STRS_MANAGE)

#
### Dynamic library adding
#
add_library(SEARCH_SUBSTR_P SHARED lib/search_substr_p/search_substr_p.c)
target_link_libraries(SEARCH_SUBSTR_P ${PTHREAD_LIBRARY} FILES_AND_STRS_MANAGE)
include_directories(lib/search_substr_p)

##
### Code coverage ###
##
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if (COVERAGE)
    set(CMAKE_BUILD_TYPE Debug)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(
    NAME coverage
    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
    EXCLUDE "build/*" "gtests/*" "src/main.c")
endif()

##
### Run settings ###
##
if (TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    include_directories(${GTEST_INCLUDE_DIRS})
    message(${GTEST_LIBRARIES})

    set(GTESTS_SRC gtests/tests.cpp)

    add_executable(tests gtests/main.cpp ${GTESTS_SRC})

    target_link_libraries(tests ${GTEST_LIBRARIES} -lpthread)
    target_link_libraries(tests FILES_AND_STRS_MANAGE)
    target_link_libraries(tests SEARCH_SUBSTR_C)
    target_link_libraries(tests SEARCH_SUBSTR_P ${PTHREAD_LIBRARY})

else()
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic -ggdb3")

    add_executable(prog src/main.c)
    target_link_libraries(prog FILES_AND_STRS_MANAGE)
    target_link_libraries(prog SEARCH_SUBSTR_C)
    target_link_libraries(prog SEARCH_SUBSTR_P ${PTHREAD_LIBRARY})
    #
    ### Linking for pthread ###
    #
    #find_library(PTHREAD_LIBRARY pthread)
    #target_link_libraries(prog ${PTHREAD_LIBRARY})
endif()
##
### Test definitions ###
##
#enable_testing()
#find_package(GTest REQUIRED)

#include_directories(${GTEST_INCLUDE_DIRS})
#message(${GTEST_LIBRARIES})

#add_executable(tests gtests/main.cpp gtests/tests.cpp)

#target_link_libraries(tests ${GTEST_LIBRARIES} -lpthread)
#target_link_libraries(tests orders_manage_lib)

