cmake_minimum_required(VERSION 3.3)
project(TinyJS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Lex.cpp Lex.h main.cpp Var.cpp Interpreter.cpp Interpreter.h Var.h)
set(MAIN  main.cpp)
set(LEX_TEST lex_test.cpp)
set(VAR_TEST var_test.cpp)
add_executable(TinyJS ${MAIN} ${SOURCE_FILES})
add_executable(LEX_TEST ${SOURCE_FILES} ${LEX_TEST})
add_executable(VAR_TEST ${SOURCE_FILES} ${VAR_TEST})
