# CSE-168 Root Level CMake Project File
# Author: Ewan Burnett (EwanBurnettSK@Outlook.com)
# Last Revised: 2024-09-26

cmake_minimum_required(VERSION 3.14)

project("CSE-168")

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    set(PLATFORM_WINDOWS 1)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_UNIX 1)
endif()

option(ENABLE_VIEWER "Enable the Viewer application" 1)
add_subdirectory(${CMAKE_SOURCE_DIR}/PathTracer/)
