cmake_minimum_required(VERSION 3.10)
project(psharpc)

file(GLOB_RECURSE SOURCES "src/*.cpp")

if(MSVC)
    add_compile_options(/W0)
else()
    add_compile_options(-w)
endif()

add_executable(psharpc ${SOURCES})