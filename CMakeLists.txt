cmake_minimum_required(VERSION 3.5)
project(parselib)

# activates debug mode 
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
# activate c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# to activate O3 optimizations
#message(BUILD TYPE="${CMAKE_BUILD_TYPE}")
if(NOT CMAKE_BUILD_TYPE)
#	message(SET RELEASE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

########################### gtest
include(FetchContent)
FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
########################### gtest

#############################
### fetch library source code
#############################
file(GLOB SOURCE_FILES
	parselib/datastructure/*.cpp
	parselib/operations/*.cpp
 	parselib/utils/*.cpp
 	parselib/parsers/*.cpp
 	parselib/parsesession.cpp
	parselib/pyparselib.cpp
)

file(GLOB HEADER_FILES
	parselib/datastructure/*.hpp
	parselib/datastructure/*.h
	parselib/operations/*.hpp
	parselib/operations/*.h
	parselib/utils/*.hpp
	parselib/utils/*.h
	parselib/parsers/*.hpp
	parselib/parsesession.hpp
)

file(GLOB TEST_FILES
	parselib/tests/*.h
)

#############################
### parselib depends on boost
#############################
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE( Boost 1.40 COMPONENTS regex system filesystem REQUIRED )

######################################
####### build parselib library #######
######################################

if(Boost_FOUND)

	include_directories(${Boost_INCLUDE_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parselib)

	add_library(${PROJECT_NAME} SHARED
		${SOURCE_FILES}
		${HEADER_FILES} parselib/datastructure/closure_types.h
            parselib/datastructure/common_types.cpp
			parselib/tests/test_template.h)
	target_link_libraries(
		${PROJECT_NAME}
		${Boost_LIBRARIES}
		${PYTHON_LIBRARIES}
	)

endif()


if(NOT BUILD_EX)
	set(BUILD_EX true)
endif()

# build test exemple if needed
if(BUILD_EX)
	set(APP parcexlib)
	add_executable(${APP} parselib/main.cpp)
	target_link_libraries(${APP}
			${PROJECT_NAME}
			${Boost_LIBRARIES}
	)
endif()

if(NOT BUILD_TEST)
	set(BUILD_TEST true)
endif()

# build test exemple if needed
if(BUILD_TEST)
	set(APP_TEST parsetestlib)
	add_executable(${APP_TEST} ${TEST_FILES} parselib/main_test.cpp)
	target_link_libraries(${APP_TEST}
			GTest::gtest_main
			${PROJECT_NAME}
			${Boost_LIBRARIES}
	)
	include(GoogleTest)
	gtest_discover_tests(${APP_TEST})
endif()
