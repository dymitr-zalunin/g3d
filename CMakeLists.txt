cmake_minimum_required(VERSION 3.0.0)
project(volleyball_court)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(thirdparty/stb_image)

set(SOURCE_FILES
    source/Helper.cpp
    source/Helper.h
    source/main.cpp
    source/tdogl/Program.cpp
    source/tdogl/Program.h
    source/tdogl/Shader.cpp
    source/tdogl/Shader.h
    source/tdogl/Texture.h
    source/tdogl/Texture.cpp
    source/tdogl/Bitmap.cpp
    source/tdogl/Bitmap.h
    source/tdogl/Camera.cpp
    source/tdogl/Camera.h)

configure_file(resources/hall-fragment-shader.txt ${EXECUTABLE_OUTPUT_PATH}/resources/hall-fragment-shader.txt COPYONLY)
configure_file(resources/hall-vertex-shader.txt ${EXECUTABLE_OUTPUT_PATH}/resources/hall-vertex-shader.txt COPYONLY)
add_executable(volleyball_court ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(volleyball_court GL glfw GLEW)