#pragma once

namespace tdogl {

    struct ModelAsset {
        tdogl::Program *shaders;
        tdogl::Texture *texture;
        GLuint vbo;
        GLuint vao;
        GLenum drawType;
        GLint drawStart;
        GLint drawCount;

        ModelAsset() :
                shaders(NULL),
                texture(NULL),
                vbo(0),
                vao(0),
                drawType(GL_TRIANGLES),
                drawStart(0),
                drawCount(0) {
        }
    };

    struct ModelInstance {
        ModelAsset *asset;
        glm::mat4 transform;

        ModelInstance() :
                asset(NULL),
                transform() {
        }
    };

}