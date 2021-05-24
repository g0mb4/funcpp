cmake_minimum_required(VERSION 3.8)
project(funcpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories( . )

add_executable(ex1 examples/ex1.cpp funcpp.h)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ex1)