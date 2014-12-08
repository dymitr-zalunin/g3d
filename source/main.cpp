/*
 main

 Copyright 2012 Thomas Dalling - http://tomdalling.com/
 Copyright 2014 Dymitr Załunin

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "Helper.h"

// third-party libraries
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <list>
// gk3d classes
#include "gk3d/Program.h"
#include "gk3d/Texture.h"
#include "gk3d/Camera.h"
#include "gk3d/Model.h"
// constants
const glm::vec2 SCREEN_SIZE(800, 600);
// globals
//gk3d::ModelAsset gCuboid;
gk3d::ModelAsset gHall , gCourt, gCuboid, gBall, gSpot, gBench;
std::list<gk3d::ModelInstance*> gInstances;
gk3d::Camera gCamera;

static void LoadAssets() {
    char const *vertexShaderFile = "scene.v.shader";
    char const *fragmentShaderFile = "scene.f.shader";
    gHall.init_cube_inward(vertexShaderFile, fragmentShaderFile);
    gHall.add_texture("wall.jpg", CUBE_UV, sizeof(CUBE_UV));

    gCourt.init(vertexShaderFile, fragmentShaderFile);
    gCourt.add_texture("court_mat.png", CUBE_UV, sizeof(CUBE_UV), 0);

    gCuboid.init(vertexShaderFile, fragmentShaderFile,glm::vec4(1.0f,1.0f,1.0f,1.0f));
    gSpot.init("spotlight.obj",vertexShaderFile,fragmentShaderFile);
    gBall.init("Volleyball.obj",vertexShaderFile,fragmentShaderFile);
    gBench.init("bench.obj",vertexShaderFile,fragmentShaderFile);
}

// convenience function that returns a translation matrix
glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
    return glm::translate(glm::mat4(), glm::vec3(x,y,z));
}


// convenience function that returns a scaling matrix
glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
    return glm::scale(glm::mat4(), glm::vec3(x,y,z));
}

// convenience function that returns a rotation matrix
glm::mat4 rotate(GLfloat x, GLfloat y, GLfloat z, GLfloat angle) {
    return glm::rotate(glm::mat4(), angle,glm::vec3(x,y,z));
}

static void CreateInstances() {

    gk3d::ModelInstance *hall=new gk3d::ModelInstance;
    hall->asset=&gHall;
//    hall->transform= translate(-10.0f,3.5f,0.0f)*scale(30.0f, 10.0f, 30.0f);
    hall->transform= translate(0.0f,33.0f,0.0f)*scale(72.0f, 40.0f, 72.0f);
    gInstances.push_back(hall);

    gk3d::ModelInstance *court =new gk3d::ModelInstance;
    court->asset=&gCourt;
    court->transform= translate(0.0f, -6.5f, 0.0f)* scale(18.0f, 0.1f, 36.0f);
    gInstances.push_back(court);

    gk3d::ModelInstance *columnRight =new gk3d::ModelInstance;
    columnRight->asset=&gCuboid;
    columnRight->transform= translate(12.0f,0.0f,0.0f)*scale(0.4,6.5,0.4);
    gInstances.push_back(columnRight);

    gk3d::ModelInstance *columnLeft=new gk3d::ModelInstance;
    columnLeft->asset=&gCuboid;
    columnLeft->transform= translate(-12.0f,0.0f,0.0f)*scale(0.4,6.5,0.4);
    gInstances.push_back(columnLeft);

    gk3d::ModelInstance *cable1=new gk3d::ModelInstance;
    cable1->asset=&gCuboid;
    cable1->transform= translate(-10.0f,2.5f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable1);
    gk3d::ModelInstance *cable2=new gk3d::ModelInstance;
    cable2->asset=&gCuboid;
    cable2->transform= translate(-10.0f,5.9f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable2);
    gk3d::ModelInstance *cable3=new gk3d::ModelInstance;
    cable3->asset=&gCuboid;
    cable3->transform= translate(10.0f,2.5f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable3);
    gk3d::ModelInstance *cable4=new gk3d::ModelInstance;
    cable4->asset=&gCuboid;
    cable4->transform= translate(10.0f,5.9f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable4);

    gk3d::ModelInstance *net=new gk3d::ModelInstance;
    net->asset=&gCuboid;
    net->transform= translate(0.0f,4.2f,0.0f)*scale(10.0,2.0,0.1);
    gInstances.push_back(net);

    gk3d::ModelInstance *ball1=new gk3d::ModelInstance;
    ball1->asset=&gBall;
    ball1->transform=translate(-7.0f, -6.5f, 10.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball1);

    gk3d::ModelInstance *ball2=new gk3d::ModelInstance;
    ball2->asset=&gBall;
    ball2->transform=translate(-5.0f, -6.5f, -13.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball2);

    gk3d::ModelInstance *ball3=new gk3d::ModelInstance;
    ball3->asset=&gBall;
    ball3->transform=translate(5.0f, -6.5f, -10.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball3);

    gk3d::ModelInstance *ball4=new gk3d::ModelInstance;
    ball4->asset=&gBall;
    ball4->transform=translate(-1.0f, -6.5f, 7.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball4);

    const float sufit = 69.0f;
    const float Z_spot = 47.0f;
    gk3d::ModelInstance *leftSpot =new gk3d::ModelInstance;
    leftSpot->asset=&gSpot;
    leftSpot->transform=translate(-60.0f, sufit, Z_spot)*rotate(0, 1, 0, 45.0f);
    gInstances.push_back(leftSpot);

    gk3d::ModelInstance *leftSpot1 =new gk3d::ModelInstance;
    leftSpot1->asset=&gSpot;
    leftSpot1->transform=translate(68.0f, sufit, Z_spot)*rotate(0, 1, 0, 135.0f);
    gInstances.push_back(leftSpot1);


    gk3d::ModelInstance *rightSpot1 =new gk3d::ModelInstance;
    rightSpot1->asset=&gSpot;
    rightSpot1->transform=translate(68.0f, sufit, -Z_spot)* rotate(0, 1, 0, -135.0f);;
    gInstances.push_back(rightSpot1);

    gk3d::ModelInstance *rightSpot2 =new gk3d::ModelInstance;
    rightSpot2->asset=&gSpot;
    rightSpot2->transform=translate(-60.0f, sufit, -Z_spot)*rotate(0, 1, 0, -45.0f);
    gInstances.push_back(rightSpot2);

    gk3d::ModelInstance *bench1 =new gk3d::ModelInstance;
    bench1->asset=&gBench;
    bench1->transform=translate(-45.0f, -5.3f, -16.0f)*scale(6,5,10);
    gInstances.push_back(bench1);

    gk3d::ModelInstance *bench2 =new gk3d::ModelInstance;
    bench2->asset=&gBench;
    bench2->transform=translate(-45.0f, -5.3f, 16.0f)*scale(6,5,10);
    gInstances.push_back(bench2);
}

// draws a single frame
static void Render() {

    glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::list<gk3d::ModelInstance*>::iterator it;
    for (it=gInstances.begin(); it!=gInstances.end(); ++it) {
        (*it)->Render(gCamera);
    }

    glfwSwapBuffers();
}

void Update(float secondsElapsed) {

    //move position of camera based on WASD keys, and XZ keys for up and down
    const float moveSpeed = 5.0; //units per second
    const float horizontalAngleSpeed = 90.0f;
    const float verticalAngleSpeed = 40.0f;
    if (glfwGetKey('S')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.forward());
    } else if (glfwGetKey('W')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.forward());
    }
    if (glfwGetKey('A')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -gCamera.right());
    } else if (glfwGetKey('D')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.right());
    }

    if (glfwGetKey('Z')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.down());
    } else if (glfwGetKey('X')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * gCamera.up());
    }

    if (glfwGetKey(GLFW_KEY_RIGHT)) {
        gCamera.offsetOrientation(0.0f, secondsElapsed * horizontalAngleSpeed);
    } else if (glfwGetKey(GLFW_KEY_LEFT)) {
        gCamera.offsetOrientation(0.0f, -secondsElapsed * horizontalAngleSpeed);
    } else if (glfwGetKey(GLFW_KEY_DOWN)) {
        gCamera.offsetOrientation(secondsElapsed * verticalAngleSpeed, 0.0f);
    } else if (glfwGetKey(GLFW_KEY_UP)) {
        gCamera.offsetOrientation(-secondsElapsed * verticalAngleSpeed, 0.0f);

    }

    //increase or decrease field of view based on mouse wheel
    const float zoomSensitivity = -2.0;
    float fieldOfView = gCamera.fieldOfView() + zoomSensitivity * (float) glfwGetMouseWheel();
    if (fieldOfView < 5.0f) fieldOfView = 5.0f;
    if (fieldOfView > 130.0f) fieldOfView = 130.0f;
    gCamera.setFieldOfView(fieldOfView);
    glfwSetMouseWheel(0);
}

void GLFWCALL reshape( int width, int height ) {
    glViewport(0, 0, width, height);
    gCamera.setViewportAspectRatio((float)width/height);
}

// the program starts here
int main(int argc, char *argv[]) {
    // initialise GLFW
    if (!glfwInit())
        throw std::runtime_error("glfwInit failed");

    // open a window with GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    if (!glfwOpenWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
        throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?");

    glfwSetWindowSizeCallback( reshape );

    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // make sure OpenGL version 3.2 API is available
    if (!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create buffer and fill it with the points of the triangle
    LoadAssets();
    CreateInstances();

    gCamera.setPosition(glm::vec3(0,13,25));
    gCamera.setNearAndFarPlanes(0.1f, 200.0f);
    gCamera.setFieldOfView(90.0f);
    gCamera.offsetOrientation(30.0f, 0.0f);
    gCamera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    // run while the window is open
    double lastTime = glfwGetTime();
    while (glfwGetWindowParam(GLFW_OPENED)) {
        // update the scene based on the time elapsed since last update
        double thisTime = glfwGetTime();
        Update(thisTime - lastTime);
        lastTime = thisTime;

        // draw one frame
        Render();

        //exit program if escape key is pressed
        if(glfwGetKey(GLFW_KEY_ESC))
            glfwCloseWindow();
    }

    // clean up and exit
    glfwTerminate();
    return EXIT_SUCCESS;
}