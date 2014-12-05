cmake_minimum_required(VERSION 3.0.0)
project(volleyball_court)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(thirdparty/stb_image thirdparty/glfw2/include thirdparty/glm thirdparty/assimp/include)

LINK_DIRECTORIES(thirdparty/glfw2/lib thirdparty/assimp/lib)

set(SOURCE_FILES
    source/Helper.cpp
    source/Helper.h
    source/main.cpp
    source/gk3d/Program.cpp
    source/gk3d/Program.h
    source/gk3d/Shader.cpp
    source/gk3d/Shader.h
    source/gk3d/Texture.h
    source/gk3d/Texture.cpp
    source/gk3d/Bitmap.cpp
    source/gk3d/Bitmap.h
    source/gk3d/Camera.cpp
    source/gk3d/Camera.h)

configure_file(resources/scene.f.shader ${EXECUTABLE_OUTPUT_PATH}/resources/scene.f.shader COPYONLY)
configure_file(resources/scene.v.shader ${EXECUTABLE_OUTPUT_PATH}/resources/scene.v.shader COPYONLY)
configure_file(resources/Volleyball.obj ${EXECUTABLE_OUTPUT_PATH}/resources/Volleyball.obj COPYONLY)
configure_file(resources/Volleyball.mtl ${EXECUTABLE_OUTPUT_PATH}/resources/Volleyball.mtl COPYONLY)
configure_file(resources/spotlight.mtl ${EXECUTABLE_OUTPUT_PATH}/resources/spotlight.mtl COPYONLY)
configure_file(resources/spotlight.obj ${EXECUTABLE_OUTPUT_PATH}/resources/spotlight.obj COPYONLY)
configure_file(resources/bench.mtl ${EXECUTABLE_OUTPUT_PATH}/resources/bench.mtl COPYONLY)
configure_file(resources/bench.obj ${EXECUTABLE_OUTPUT_PATH}/resources/bench.obj COPYONLY)
add_executable(volleyball_court ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(volleyball_court GL glfw GLEW assimp)
