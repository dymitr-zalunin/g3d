#pragma once

#include "Shader.h"
#include "Program.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

namespace tdogl {

    static GLfloat RECTANGLE[]={
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
          };

    static GLfloat CUBE[] = {
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

    struct ModelAsset {
        tdogl::Program *shaders;
        std::vector<Texture*> textures;
        GLuint vbo;
        GLuint vao;
        GLenum drawType;
        GLint drawStart;
        GLint drawCount;

        ModelAsset() :
                shaders(NULL),
                textures(),
                vbo(0),
                vao(0),
                drawType(GL_TRIANGLES),
                drawStart(0),
                drawCount(0) {
        }

        virtual void init(const char *fragmentFile, const char *vertexFile) = 0;

        virtual void init(const char* filename) {
            std::cout << "Init from 3d model file. Not implement." << std::cout;
        }

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

    };

    struct ModelInstance {
        ModelAsset *asset;
        glm::mat4 transform;

        ModelInstance() :
                asset(NULL),
                transform() {
        }

        virtual void Render(const Camera &gCamera) const = 0;

    };

    class HallAsset : public ModelAsset {
    public:
        HallAsset() :
                ModelAsset() {

        }

        void init(const char *fragmentFile, const char *vertexFile) {
            this->shaders = ModelAsset::LoadShaders(vertexFile, fragmentFile);
            this->drawType = GL_TRIANGLES;
            this->drawStart = 0;
            this->drawCount = 6 * 3 * 2;
            glGenBuffers(1, &this->vbo);
            glGenVertexArrays(1, &this->vao);

            glBindVertexArray(this->vao);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

            glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), CUBE, GL_STATIC_DRAW);

            // connect the xyz to the "vert" attribute of the vertex shader
            glEnableVertexAttribArray(this->shaders->attrib("vert"));
            glVertexAttribPointer(this->shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

            // unbind the VAO
            glBindVertexArray(0);
        }
    };

    class ColumnAsset : public ModelAsset {
    public:
        ColumnAsset() :
                ModelAsset() {

        }

        void init(const char *fragmentFile, const char *vertexFile) {
            this->shaders = ModelAsset::LoadShaders(vertexFile, fragmentFile);
            this->drawType = GL_TRIANGLES;
            this->drawStart = 0;
            this->drawCount = 6 * 3 * 2;
            glGenBuffers(1, &this->vbo);
            glGenVertexArrays(1, &this->vao);

            glBindVertexArray(this->vao);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

            glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), CUBE, GL_STATIC_DRAW);

            // connect the xyz to the "vert" attribute of the vertex shader
            glEnableVertexAttribArray(this->shaders->attrib("vert"));
            glVertexAttribPointer(this->shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

            // unbind the VAO
            glBindVertexArray(0);
        }
    };

    class HallInstance : public ModelInstance {
    public:
        HallInstance():
                ModelInstance()
        {}

        void Render(const Camera &gCamera) const {
            tdogl::ModelAsset* asset = this->asset;
            tdogl::Program* shaders = asset->shaders;

            //bind the shaders
            shaders->use();

            //set the shader uniforms
            shaders->setUniform("camera", gCamera.matrix());
            shaders->setUniform("model", this->transform);

            if (asset->textures.size()>0) {
                //bind the texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, asset->textures.front()->object());
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
    };

    class ColumnInstance : public ModelInstance {
    public:
        ColumnInstance():
                ModelInstance()
        {}

        void Render(const Camera &gCamera) const {
            tdogl::ModelAsset* asset = this->asset;
            tdogl::Program* shaders = asset->shaders;

            //bind the shaders
            shaders->use();

            //set the shader uniforms
            shaders->setUniform("camera", gCamera.matrix());
            shaders->setUniform("model", this->transform);

            //bind VAO and draw
            glBindVertexArray(asset->vao);
            glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

            //unbind everything
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            shaders->stopUsing();
        }
    };
}