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
// tdogl classes
#include "tdogl/Program.h"
#include "tdogl/Texture.h"
#include "tdogl/Camera.h"
#include "tdogl/Model.h"
// constants
const glm::vec2 SCREEN_SIZE(800, 600);
// globals
//tdogl::ModelAsset gCuboid;
tdogl::ModelAsset gHall ,gCuboid, gBall, gSpot, gBench;
std::list<tdogl::ModelInstance*> gInstances;
tdogl::Camera gCamera;

static void LoadAssets() {
    char const *vertexShaderFile = "scene.v.shader";
    char const *fragmentShaderFile = "scene.f.shader";
    gHall.init_cube_inward(vertexShaderFile, fragmentShaderFile,glm::vec4(0.72156862745f,0.01176470588f,1.0f,1.0f));
    gCuboid.init(vertexShaderFile, fragmentShaderFile);
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

    tdogl::ModelInstance *hall=new tdogl::ModelInstance;
    hall->asset=&gHall;
    hall->transform= translate(-10.0f,3.5f,0.0f)*scale(30.0f, 10.0f, 30.0f);
    gInstances.push_back(hall);

    tdogl::ModelInstance *hall1=new tdogl::ModelInstance;
    hall1->asset=&gCuboid;
    hall1->transform= translate(0.0f, -6.5f, 0.0f)* scale(12.0f, 0.1f, 20.0f);
    gInstances.push_back(hall1);

    tdogl::ModelInstance *columnRight =new tdogl::ModelInstance;
    columnRight->asset=&gCuboid;
    columnRight->transform= translate(12.0f,0.0f,0.0f)*scale(0.4,6.5,0.4);
    gInstances.push_back(columnRight);

    tdogl::ModelInstance *columnLeft=new tdogl::ModelInstance;
    columnLeft->asset=&gCuboid;
    columnLeft->transform= translate(-12.0f,0.0f,0.0f)*scale(0.4,6.5,0.4);
    gInstances.push_back(columnLeft);

    tdogl::ModelInstance *cable1=new tdogl::ModelInstance;
    cable1->asset=&gCuboid;
    cable1->transform= translate(-10.0f,2.5f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable1);
    tdogl::ModelInstance *cable2=new tdogl::ModelInstance;
    cable2->asset=&gCuboid;
    cable2->transform= translate(-10.0f,5.9f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable2);
    tdogl::ModelInstance *cable3=new tdogl::ModelInstance;
    cable3->asset=&gCuboid;
    cable3->transform= translate(10.0f,2.5f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable3);
    tdogl::ModelInstance *cable4=new tdogl::ModelInstance;
    cable4->asset=&gCuboid;
    cable4->transform= translate(10.0f,5.9f,0.0f)*scale(2.0,0.1,0.1);
    gInstances.push_back(cable4);

    tdogl::ModelInstance *net=new tdogl::ModelInstance;
    net->asset=&gCuboid;
    net->transform= translate(0.0f,4.2f,0.0f)*scale(10.0,2.0,0.1);
    gInstances.push_back(net);

    tdogl::ModelInstance *ball1=new tdogl::ModelInstance;
    ball1->asset=&gBall;
    ball1->transform=translate(-7.0f, -6.5f, 10.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball1);

    tdogl::ModelInstance *ball2=new tdogl::ModelInstance;
    ball2->asset=&gBall;
    ball2->transform=translate(-5.0f, -6.5f, -13.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball2);

    tdogl::ModelInstance *ball3=new tdogl::ModelInstance;
    ball3->asset=&gBall;
    ball3->transform=translate(5.0f, -6.5f, -10.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball3);

    tdogl::ModelInstance *ball4=new tdogl::ModelInstance;
    ball4->asset=&gBall;
    ball4->transform=translate(-1.0f, -6.5f, 7.0f)*scale(0.2,0.2,0.2);
    gInstances.push_back(ball4);

    const float sufit = 9.5f;
    tdogl::ModelInstance *leftSpot =new tdogl::ModelInstance;
    leftSpot->asset=&gSpot;
    leftSpot->transform=translate(-20.0f, sufit, 27.0f)*rotate(0, 1, 0, 45.0f);
    gInstances.push_back(leftSpot);

    tdogl::ModelInstance *leftSpot1 =new tdogl::ModelInstance;
    leftSpot1->asset=&gSpot;
    leftSpot1->transform=translate(18.0f, sufit, 27.0f)*rotate(0, 1, 0, 135.0f);
    gInstances.push_back(leftSpot1);


    tdogl::ModelInstance *rightSpot1 =new tdogl::ModelInstance;
    rightSpot1->asset=&gSpot;
    rightSpot1->transform=translate(18.0f, sufit, -27.0f)* rotate(0, 1, 0, -135.0f);;
    gInstances.push_back(rightSpot1);

    tdogl::ModelInstance *rightSpot2 =new tdogl::ModelInstance;
    rightSpot2->asset=&gSpot;
    rightSpot2->transform=translate(-20.0f, sufit, -27.0f)*rotate(0, 1, 0, -45.0f);
    gInstances.push_back(rightSpot2);

    tdogl::ModelInstance *bench1 =new tdogl::ModelInstance;
    bench1->asset=&gBench;
    bench1->transform=translate(-30.0f, -5.3f, -16.0f)*scale(6,5,10);
    gInstances.push_back(bench1);

    tdogl::ModelInstance *bench2 =new tdogl::ModelInstance;
    bench2->asset=&gBench;
    bench2->transform=translate(-30.0f, -5.3f, 16.0f)*scale(6,5,10);
    gInstances.push_back(bench2);
}

// draws a single frame
static void Render() {

    glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::list<tdogl::ModelInstance*>::iterator it;
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