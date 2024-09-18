# CSE-168 Root Level CMake Project File
# Author: Ewan Burnett (EwanBurnettSK@Outlook.com)
# Last Revised: 2024-09-26

cmake_minimum_required(VERSION 3.14)

project("CSE-168")

set(CMAKE_CXX_STANDARD 17)

option(BUILD_VIEWER "Enable the Viewer application" ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/PathTracer/)
