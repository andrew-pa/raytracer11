cmake_minimum_required(VERSION 3.0)

project(raytracer11)


include_directories("${PROJECT_SOURCE_DIR}/inc")
file(GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(raytracer11 ${SOURCE})
set_property(TARGET raytracer11 PROPERTY CXX_STANDARD 14)

