cmake_minimum_required(VERSION 2.6)
project(parselib)

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON)

find_package(Boost 1.58 COMPONENTS regex system filesystem) 

file(GLOB_RECURSE SOURCE_FILES
	parselib/datastructure/*.cpp
 	parselib/operations/*.cpp
 	parselib/utils/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
	parselib/datastructure/*.hpp
	parselib/operations/*.hpp
	parselib/utils/*.hpp
)

set(APP parcexlib)

# parselib depends on boost
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parselib)

	add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES}  ${Boost_LIBRARIES})
	set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/parselib)

	add_executable(${APP} main.cpp)
	target_link_libraries(${APP} parselib ${Boost_LIBRARIES})
endif()


