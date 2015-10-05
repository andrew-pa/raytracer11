cmake_minimum_required(VERSION 3.0)

project(raytracer11)

if(MINGW)
  message("MINGW")
  add_definitions("-DMINGW")
elseif(WIN32)
  include_directories($ENV{HOME}/Source/share/include)
endif()

set(ASSIMP_DIR ${PROJECT_SOURCE_DIR}/depd/assimp)

include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${ASSIMP_DIR}/include)

file(GLOB SOIL_SOURCE src/soil/* inc/soil/*.h)
add_library(soil ${SOIL_SOURCE})

file(GLOB SOURCE src/*.cpp inc/*.h)
add_executable(raytracer11 ${SOURCE})
set_property(TARGET raytracer11 PROPERTY CXX_STANDARD 14)
target_link_libraries(raytracer11 soil ${ASSIMP_DIR}/build/code/$<CONFIG>/assimp*.lib)
