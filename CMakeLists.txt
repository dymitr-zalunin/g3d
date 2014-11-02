cmake_minimum_required(VERSION 3.0.0)
project(volleyball_court)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(SHADERS
    fragment-shader.txt
    vertex-shader.txt)

set(SOURCE_FILES
    source/Helper.cpp
    source/Helper.h
    source/main.cpp
    source/Program.cpp
    source/Program.h
    source/Shader.cpp
    source/Shader.h)

configure_file(resources/fragment-shader.txt ${EXECUTABLE_OUTPUT_PATH}/resources/fragment-shader.txt)
configure_file(resources/vertex-shader.txt ${EXECUTABLE_OUTPUT_PATH}/resources/vertex-shader.txt)
add_executable(volleyball_court ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(volleyball_court GL glfw GLEW)