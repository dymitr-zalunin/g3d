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
tdogl::ModelAsset gHall;
std::list<tdogl::ModelInstance> gInstances;
tdogl::Camera gCamera;
GLfloat gDegreesRotated = 0.0f;


// returns the full path to the file `fileName` in the resources directory of the app bundle
static std::string ResourcePath(std::string fileName) {
    return GetProcessPath() + "/resources/" + fileName;
}


// loads the vertex shader and fragment shader, and links them to make the global gProgram
static tdogl::Program *LoadShaders(const char *vertexFilename, const char *fragmentFilename) {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(vertexFilename), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath(fragmentFilename), GL_FRAGMENT_SHADER));
    return new tdogl::Program(shaders);
}


// loads the content from file `filename` into gTexture
static tdogl::Texture *LoadTexture(const char *filename) {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath(filename));
    bmp.flipVertically();
    return new tdogl::Texture(bmp);
}

static void LoadHallAsset() {
    gHall.shaders = LoadShaders("hall-vertex-shader.txt", "hall-fragment-shader.txt");
    gHall.drawType = GL_TRIANGLES;
    gHall.drawStart = 0;
    gHall.drawCount = 6 * 3 * 2;
    glGenBuffers(1, &gHall.vbo);
    glGenVertexArrays(1, &gHall.vao);

    glBindVertexArray(gHall.vao);
    glBindBuffer(GL_ARRAY_BUFFER, gHall.vbo);

    GLfloat vertexData[] = {
            //  X     Y     Z
            // bottom
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            // top
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,

            // front
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,

            // back
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,

            // left
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            // right
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,

    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData),vertexData, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gHall.shaders->attrib("vert"));
    glVertexAttribPointer(gHall.shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);

    // unbind the VAO
    glBindVertexArray(0);
}

// convenience function that returns a translation matrix
glm::mat4 translate(GLfloat x, GLfloat y, GLfloat z) {
    return glm::translate(glm::mat4(), glm::vec3(x,y,z));
}


// convenience function that returns a scaling matrix
glm::mat4 scale(GLfloat x, GLfloat y, GLfloat z) {
    return glm::scale(glm::mat4(), glm::vec3(x,y,z));
}

static void CreateInstances() {
    tdogl::ModelInstance hall;
    hall.asset=&gHall;
    hall.transform=scale(8,6,10);
    gInstances.push_back(hall);
}

static void RenderInstance(const tdogl::ModelInstance& inst) {
    tdogl::ModelAsset* asset = inst.asset;
    tdogl::Program* shaders = asset->shaders;

    //bind the shaders
    shaders->use();

    //set the shader uniforms
    shaders->setUniform("camera", gCamera.matrix());
    shaders->setUniform("model", inst.transform);

    if (asset->texture!= NULL) {
        //bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, asset->texture->object());
        shaders->setUniform("tex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0
    }

    //bind VAO and draw
    glBindVertexArray(asset->vao);
    glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

    //unbind everything
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shaders->stopUsing();
}

// draws a single frame
static void Render() {

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::list<tdogl::ModelInstance>::const_iterator it;
    for (it=gInstances.begin(); it!=gInstances.end(); ++it) {
        RenderInstance(*it);
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
        gCamera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0, 1, 0));
    } else if (glfwGetKey('X')) {
        gCamera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0, 1, 0));
    } else if (glfwGetKey(GLFW_KEY_RIGHT)) {
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

// the program starts here
int main(int argc, char *argv[]) {
    // initialise GLFW
    if (!glfwInit())
        throw std::runtime_error("glfwInit failed");

    // open a window with GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (!glfwOpenWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
        throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?");

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
    LoadHallAsset();
    CreateInstances();

    gCamera.setPosition(glm::vec3(-10, 10, 24));
    gCamera.offsetOrientation(25.0f, 23.0f);
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