cmake_minimum_required(VERSION 3.18.1)

project(csgo-restart VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOURCES "${SRC_DIR}/main.cpp")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${SRC_DIR}/config.h)
add_executable(${PROJECT_NAME} ${SOURCES})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
