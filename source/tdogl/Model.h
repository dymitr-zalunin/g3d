#pragma once

#include "Shader.h"
#include "Program.h"

#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <GL/glext.h>
#include <GL/gl.h>

namespace tdogl {

    static GLfloat CUBE[] = {
            //  X     Y     Z  Normal
            // bottom
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f,

            // top
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

            // front
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

            // back
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,

            // left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,

            // right
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
    };

    struct Mesh {
        GLuint vbo;
        GLuint vao;
        glm::vec4 ambientColor;
        glm::vec4 diffuseColor;
        glm::vec4 specularColor;
        tdogl::Program *shaders;
        float shininess;
        GLenum drawType;
        GLint drawStart;
        GLint drawCount;

        Mesh() :
                vao(0),
                vbo(0),
                ambientColor(glm::vec4(1.0f, 1.0f, 1.0f,1.0f)),
                diffuseColor(glm::vec4(1.0f, 1.0f, 1.0f,1.0f)),
                specularColor(glm::vec4(1.0f, 1.0f, 1.0f,1.0f)),
                shininess(1),
                shaders(NULL),
                drawStart(0),
                drawCount(0),
                drawType(GL_TRIANGLES) {
        }
    };

    struct ModelAsset {
        tdogl::Program *shaders;
        std::vector<Texture *> textures;
        std::vector<Mesh *> meshes;

        ModelAsset() :
                shaders(NULL),
                textures(),
                meshes() {
        }

        void init(const char *vertexFile, const char *fragmentFile) {
            Mesh *aMesh = create_mesh(vertexFile, fragmentFile, 36, sizeof(CUBE), CUBE);
            this->meshes.push_back(aMesh);
        }

        void init(const char *modelFile, const char *vertexFile, const char *fragmentFile) {

            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(ResourcePath(modelFile),
                    aiProcess_Triangulate |
                            aiProcess_GenNormals);
            if (scene) {

                std::vector<GLfloat> vertexList;

                unsigned int nMeshes = scene->mNumMeshes;
                for (int i = 0; i < nMeshes; ++i) {
                    aiMesh *ai_mesh = scene->mMeshes[i];
                    get_vertices(ai_mesh, vertexList);

                    aiMaterial *material = scene->mMaterials[ai_mesh->mMaterialIndex];

                    Mesh *aMesh = create_mesh(vertexFile, fragmentFile, vertexList.size() / 3, sizeof(float) * vertexList.size(), &vertexList.front());
                    aMesh->ambientColor = get_material_color(material, AI_MATKEY_COLOR_AMBIENT);
                    aMesh->diffuseColor = get_material_color(material, AI_MATKEY_COLOR_DIFFUSE);
                    aMesh->specularColor = get_material_color(material, AI_MATKEY_COLOR_SPECULAR);

                    float shininess;
                    unsigned int max = 1;
                    aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &max);

                    if (shininess != 0) {
                        aMesh->shininess = shininess;
                    }

                    vertexList.clear();
                    this->meshes.push_back(aMesh);

                }
            } else {
                std::cout << "Failed! Error: " << importer.GetErrorString() << std::endl;
            }
        }

        Mesh *create_mesh(char const *vertexFile, char const *fragmentFile, int size, GLsizeiptr ptrSize, GLfloat *array) {
            Mesh *aMesh = new Mesh;
            aMesh->shaders = LoadShaders(vertexFile, fragmentFile);
            aMesh->drawType = GL_TRIANGLES;
            aMesh->drawStart = 0;
            aMesh->drawCount = size;
            glGenBuffers(1, &aMesh->vbo);
            glGenVertexArrays(1, &aMesh->vao);

            glBindVertexArray(aMesh->vao);
            glBindBuffer(GL_ARRAY_BUFFER, aMesh->vbo);

            glBindVertexArray(aMesh->vao);
            glBindBuffer(GL_ARRAY_BUFFER, aMesh->vbo);

            glBufferData(GL_ARRAY_BUFFER, ptrSize, array, GL_STATIC_DRAW);

            // connect the xyz to the "vert" attribute of the vertex shader
            glEnableVertexAttribArray(aMesh->shaders->attrib("vert"));
            glVertexAttribPointer(aMesh->shaders->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);

            glEnableVertexAttribArray(aMesh->shaders->attrib("vertNormal"));
            glVertexAttribPointer(aMesh->shaders->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const GLvoid *) (3 * sizeof(GLfloat)));

            // unbind the VAO
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return aMesh;
        }

        void get_vertices(const aiMesh *mesh, std::vector<GLfloat> &vertices) {

            int nFaces = mesh->mNumFaces;
            for (int j = 0; j < nFaces; ++j) {
                const aiFace &face = mesh->mFaces[j];
                for (int k = 0; k < 3; ++k) {
                    aiVector3D pos = mesh->mVertices[face.mIndices[k]];
                    aiVector3D normal = mesh->mNormals[face.mIndices[k]];
//                            aiVector3D uv =mesh->mTextureCoords[0][face.mIndices[k]];
                    vertices.push_back(pos.x);
                    vertices.push_back(pos.y);
                    vertices.push_back(pos.z);
                    vertices.push_back(normal.x);
                    vertices.push_back(normal.y);
                    vertices.push_back(normal.z);
//                            vertexList.push_back(uv.x);
//                            vertexList.push_back(uv.y);
//                            vertexList.push_back(uv.z);
//                            aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
                }
            }
        }

        glm::vec4 get_material_color(const aiMaterial *material, char const *pKey, unsigned int type, unsigned int index) {
            aiColor4D color;
            aiGetMaterialColor(material, pKey, type, index, &color);
            return glm::vec4(color.r, color.g, color.b,color.a);
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

        struct Light {
            glm::vec4 position;
            glm::vec3 intensities; //a.k.a. the color of the light
            float attenuation;
            float ambientCoefficient;
            float coneAngle;
            glm::vec3 coneDirection;
        };

        ModelAsset *asset;
        glm::mat4 transform;
        std::vector<Light> lights;

        ModelInstance() :
                asset(NULL),
                transform() {

            Light spotlight1;
            spotlight1.intensities = glm::vec3(1, 1, 1);
            spotlight1.position = glm::vec4(-3.1, 25.5, 10.0,1);
            spotlight1.attenuation = 0.0f;
            spotlight1.ambientCoefficient = 0.0f;
            spotlight1.coneAngle=40.0f;
            spotlight1.coneDirection=glm::vec3(0,-1,0);

            Light spotlight2;
            spotlight2.intensities = glm::vec3(1, 0, 0);
            spotlight2.position = glm::vec4(3.1, 25.5, -14.0,1);
            spotlight2.attenuation = 0.0f;
            spotlight2.ambientCoefficient = 0.0f;
            spotlight2.coneAngle=40.0f;
            spotlight2.coneDirection=glm::vec3(0,-1,0);

            Light directional_light;
            directional_light.intensities = glm::vec3(1, 1, 1);
            directional_light.position = glm::vec4(0, 40, 0, 0);
            directional_light.attenuation = 0.5f;
            directional_light.ambientCoefficient = 0.0f;

            Light directional_light1;
            directional_light1.intensities = glm::vec3(1, 1, 1);
            directional_light1.position = glm::vec4(0, 40, 20, 0);
            directional_light1.attenuation = 0.5f;
            directional_light1.ambientCoefficient = 0.0f;

            lights=std::vector<Light>();
//            lights.push_back(spotlight1);
//            lights.push_back(spotlight2);
            lights.push_back(directional_light);
            lights.push_back(directional_light1);
        }

//        virtual void Render(const Camera &gCamera) const = 0;

        void Render(const Camera &gCamera) const {

            tdogl::ModelAsset *asset = this->asset;
            for (int i = 0; i < asset->meshes.size(); ++i) {

                tdogl::Mesh *mesh = asset->meshes[i];
                tdogl::Program *shaders = mesh->shaders;

                //bind the shaders
                shaders->use();

                //set the shader uniforms
                shaders->setUniform("camera", gCamera.matrix());

                shaders->setUniform("model", this->transform);

                shaders->setUniform("materialAmbientColor", mesh->ambientColor);
                shaders->setUniform("materialDiffuseColor", mesh->diffuseColor);
                shaders->setUniform("materialSpecularColor", mesh->specularColor);
                shaders->setUniform("materialShininess", mesh->shininess);

                shaders->setUniform("numLights", (int)lights.size());
                for(size_t i = 0; i < lights.size(); ++i) {
                    SetLightUniform(shaders, "position", i, lights[i].position);
                    SetLightUniform(shaders, "intensities", i, lights[i].intensities);
                    SetLightUniform(shaders, "attenuation", i, lights[i].attenuation);
                    SetLightUniform(shaders, "ambientCoefficient", i, lights[i].ambientCoefficient);
                    SetLightUniform(shaders, "coneAngle", i, lights[i].coneAngle);
                    SetLightUniform(shaders, "coneDirection", i, lights[i].coneDirection);
                }

//                shaders->setUniform("light.position", glm::vec4(light.position, 0));
//                shaders->setUniform("light.intensities", light.intensities);
//                shaders->setUniform("light.attenuation", light.attenuation);
//                shaders->setUniform("light.ambientCoefficient", light.ambientCoefficient);
//                shaders->setUniform("light.coneAngle", 20.0f);
//                shaders->setUniform("light.coneDirection", glm::vec3(0.0, -1.0, 0.0));

                //bind VAO and draw
                glBindVertexArray(mesh->vao);
                glDrawArrays(mesh->drawType, mesh->drawStart, mesh->drawCount);

                //unbind everything
                glBindVertexArray(0);
                shaders->stopUsing();
            }
        }

        template <typename T>
        void SetLightUniform(tdogl::Program* shaders, const char* propertyName, size_t lightIndex, T& value) const{
            std::ostringstream ss;
            ss << "lights[" << lightIndex << "]." << propertyName;
            std::string uniformName = ss.str();

            shaders->setUniform(uniformName.c_str(), value);
        }

    };

}