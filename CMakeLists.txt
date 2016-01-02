cmake_minimum_required(VERSION 3.3)
project(TinyJS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Lex.cpp Lex.h main.cpp var.cpp TinyJS.cpp TinyJS.h var.h)
add_executable(TinyJS ${SOURCE_FILES})