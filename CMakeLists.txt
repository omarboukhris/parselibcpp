cmake_minimum_required(VERSION 3.5)
project(parselib)

# activates debug mode 
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
# activate c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#############################
### fetch library source code
#############################
file(GLOB SOURCE_FILES
	parselib/datastructure/*.cpp
 	parselib/operations/*.cpp
 	parselib/utils/*.cpp
 	parselib/parsers/*.cpp
 	parselib/*.cpp
)

file(GLOB HEADER_FILES
	parselib/datastructure/*.hpp
	parselib/operations/*.hpp
	parselib/utils/*.hpp
	parselib/parsers/*.hpp
	parselib/*.hpp
)

#############################
### parselib depends on boost
#############################
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE( Boost 1.40 COMPONENTS regex system filesystem REQUIRED )

# find_package(Boost 1.58 COMPONENTS regex system filesystem) 

######################################
####### build parselib library #######
######################################

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR}) 
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parselib)

	add_library(${PROJECT_NAME} STATIC 
		${SOURCE_FILES} 
		${HEADER_FILES} )
	target_link_libraries(
		${PROJECT_NAME}
		${Boost_LIBRARIES}
	)
endif()

# #######################################
# ####### build generator library #######
# #######################################
# file (GLOB GENERATOR_HEADER_FILES
# 	generator/generator.hpp
# )
# file (GLOB GENERATOR_SOURCE_FILES
# 	generator/generator.cpp
# )
# 
# set(GENERATOR_PROJECT_NAME generator)
# if(Boost_FOUND)
# 	include_directories(${Boost_INCLUDE_DIRS}) 
# 	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# 	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/generator)
# 
# 	#build library
# 	add_library(${GENERATOR_PROJECT_NAME} STATIC 
# 		${GENERATOR_SOURCE_FILES} ${GENERATOR_HEADER_FILES} 
# 		${PROJECT_NAME} 
# 		${Boost_LIBRARIES}
# 	)
# 	
# endif()
# 
# #######################################
# ####### Build example if needed #######
# #######################################
# 
# choose if you want to build the example or not
if(NOT BUILD_EX)
    set(BUILD_EX true)
endif()

# build test exemple if needed
if(BUILD_EX)
	set(APP parcexlib)
	add_executable(${APP} main.cpp)
	target_link_libraries(${APP} 
		${PROJECT_NAME} 
		${GENERATOR_PROJECT_NAME} 
		${Boost_LIBRARIES}
	)
endif()
