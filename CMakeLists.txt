cmake_minimum_required(VERSION 3.0)

project(raytracer11)

if(WIN32)
  include_directories($ENV{HOME}/Source/share/include)
elseif(MINGW)
  add_definitions("-DMINGW")
endif()

include_directories("${PROJECT_SOURCE_DIR}/inc")
file(GLOB SOURCE src/*.cpp inc/*.h)
add_executable(raytracer11 ${SOURCE})
set_property(TARGET raytracer11 PROPERTY CXX_STANDARD 14)
