cmake_minimum_required(VERSION 3.6)
project(ash)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES shell.c shell.h parseline.c promptline.c execute.c execute.h)

add_executable(ash ${SOURCE_FILES})

set_target_properties(ash PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)