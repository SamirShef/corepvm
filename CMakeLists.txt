cmake_minimum_required(VERSION 3.10)
project(psharp)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(psharp ${SOURCES})