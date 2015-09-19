cmake_minimum_required(VERSION 3.0)

project(raytracer11)


include_directories($ENV{HOME}/Source/share/include)

include_directories("${PROJECT_SOURCE_DIR}/inc")
file(GLOB SOURCE src/*.cpp inc/*.h)
add_executable(raytracer11 ${SOURCE})
