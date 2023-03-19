cmake_minimum_required(VERSION 3.5)
project(parselib)

# activates debug mode 
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
# activate c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# to activate O3 optimizations
#if(NOT CMAKE_BUILD_TYPE)
#	set(CMAKE_BUILD_TYPE Release)
#endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#############################
### fetch library source code
#############################
file(GLOB SOURCE_FILES
	parselib/datastructure/*.cpp
	parselib/datastructure/operations/*.cpp
	parselib/operations/*.cpp
 	parselib/utils/*.cpp
 	parselib/parsers/*.cpp
 	parselib/*.cpp
)

file(GLOB HEADER_FILES
	parselib/datastructure/*.hpp
	parselib/datastructure/*.h
	parselib/datastructure/operations/*.hpp
	parselib/operations/*.hpp
	parselib/utils/*.hpp
	parselib/parsers/*.hpp
	parselib/*.hpp
)

list(REMOVE_ITEM SOURCE_FILES "parselib/main.cpp")

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
		${HEADER_FILES} parselib/datastructure/closure_types.h)
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
