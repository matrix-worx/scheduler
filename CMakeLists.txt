cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(scheduler CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_sources
    CComputationNode.hpp
    CComputationNode.cpp
    main.cpp
)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

if(WIN32)
	find_package(Boost
		REQUIRED
		COMPONENTS system program_options regex thread date_time chrono
	)
else()
	find_package(Boost
		REQUIRED
		COMPONENTS system program_options regex thread)
endif()
find_package (Threads REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

if(NOT WIN32)
	include(CheckCXXCompilerFlag)

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	  CHECK_CXX_COMPILER_FLAG("-std=c++11" STDCPP11_SUPPORTED)
	  if(STDCPP11_SUPPORTED)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	  else()
		CHECK_CXX_COMPILER_FLAG("-std=c++0x" STDCPP0X_SUPPORTED)
		if(STDCPP0X_SUPPORTED)
		  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
		else()
		  message(FATAL_ERROR "Compiler doesn't appear to support C++11 or at least C++0x")
		endif()
	  endif()
	  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -ggdb3 -Wall -Wextra -pedantic")
	  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
	else()
	  message(FATAL_ERROR "Unsupported compiler")
	endif()
endif()

message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")

add_subdirectory(libmatrix ${CMAKE_BINARY_DIR}/libmatrix/ EXCLUDE_FROM_ALL)
add_subdirectory(jsoncpp EXCLUDE_FROM_ALL)

include_directories(gsl
                    ${CMAKE_BINARY_DIR}/gsl/)

include_directories(jsoncpp
                    ${CMAKE_BINARY_DIR}/jsoncpp/include)

include_directories(libmatrix
                    ${CMAKE_BINARY_DIR}/libmatrix/)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(scheduler
               ${project_sources}
)

target_link_libraries(scheduler
    matrix
    jsoncpp_lib_static
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
