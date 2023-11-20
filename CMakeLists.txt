cmake_minimum_required(VERSION 3.25)
project(JudgeTestBugReproducible)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(JudgeTestBugReproducible main.cpp
        main.h)
