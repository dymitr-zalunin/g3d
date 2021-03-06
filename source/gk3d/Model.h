#pragma once

#include "Shader.h"
#include "Program.h"
#include "Cube.h"

#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <GL/glext.h>
#include <GL/gl.h>

namespace gk3d {

    struct Fog{
        float density;
        /**
        * 0 - exp
        * 1 - exp2
        * 2 - linear
        */
        int eq;
        glm::vec4 color;
        float start;
        float end;
    };

    struct RenderParams {
        GLint magTextureFilter;
        GLint minTextureFilter;
        GLfloat bias;
        Fog* fog;
    };

    struct Mesh {
        GLuint vbo;
        GLuint vao;
        GLuint texVbo;
        glm::vec4 ambientColor;
        glm::vec4 diffuseColor;
        glm::vec4 specularColor;
        gk3d::Program *shaders;
        std::vector<Texture *> textures;
        Texture* swap;
        int swap_ind;
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
                textures(),
                swap(NULL),
                swap_ind(0),
                shaders(NULL),
                drawStart(0),
                drawCount(0),
                drawType(GL_TRIANGLES) {
        }
    };

    struct ModelAsset {
        std::vector<Mesh *> meshes;

        ModelAsset() :
                meshes() {
        }

        void init_cube_inward(const char *vertexFile, const char *fragmentFile, glm::vec4 materialDiffuseColor=glm::vec4(1.0f,1.0f,1.0f,1.0f)) {
            Mesh *aMesh = create_mesh(vertexFile, fragmentFile, 36, sizeof(CUBE_INWARD), CUBE_INWARD,materialDiffuseColor);
            this->meshes.push_back(aMesh);
        }

        /**
        * Added texture from file @filename to mesh at the index @index(defaut 0).
        */
        void add_texture(const char* filename, GLfloat uv[], GLsizeiptr ptrSize, int index =0, GLint minMagFiler = GL_LINEAR, GLint wrapMode = GL_CLAMP_TO_EDGE) {
            assert(this->meshes.size()>0);
            Bitmap bitmap=Bitmap::bitmapFromFile(ResourcePath(filename));
            Texture *texture=new Texture(bitmap,minMagFiler,wrapMode);
            Mesh *aMesh=this->meshes[index];
            aMesh->textures.push_back(texture);
            load_textures(aMesh,uv,ptrSize);
        }
        
        void save_texture_to_swap(int n) {
            std::vector<Texture *>& tex = this->meshes[0]->textures;
            assert(tex.size()>n);
            this->meshes[0]->swap= tex[n];
            this->meshes[0]->swap_ind=n;
            tex.erase(tex.begin()+n);
        }

        void flush_swap() {
            Mesh *mesh = this->meshes[0];
            if (mesh->swap!=NULL) {
                mesh->textures.insert(mesh->textures.begin()+mesh->swap_ind,mesh->swap);
                mesh->swap=NULL;
                mesh->swap_ind=-1;
            }
        }

        Texture* swap() {
            return this->meshes[0]->swap;
        }

        void init(const char *vertexFile, const char *fragmentFile, glm::vec4 materialDiffuseColor=glm::vec4(1.0f,1.0f,1.0f,1.0f)) {
            Mesh *aMesh = create_mesh(vertexFile, fragmentFile, 36, sizeof(CUBE), CUBE,materialDiffuseColor);
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

        Mesh *create_mesh(char const *vertexFile, char const *fragmentFile, int size, GLsizeiptr ptrSize, GLfloat *array,glm::vec4 materialDiffuseColor=glm::vec4(1.0f,1.0f,1.0f,1.0f)) {
            Mesh *aMesh = new Mesh;
            aMesh->diffuseColor=materialDiffuseColor;
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

        void load_textures(Mesh *mesh, GLfloat uv[], GLsizeiptr ptrSize) {
            glGenBuffers(1,&mesh->texVbo);

            glBindVertexArray(mesh->vao);
            glBindBuffer(GL_ARRAY_BUFFER,mesh->texVbo);

            glBufferData(GL_ARRAY_BUFFER,ptrSize,uv, GL_STATIC_DRAW);
            glEnableVertexAttribArray(mesh->shaders->attrib("vertTexCoord"));
            glVertexAttribPointer(mesh->shaders->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), NULL);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
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
        static gk3d::Program *LoadShaders(const char *vertexFilename, const char *fragmentFilename) {
            std::vector<gk3d::Shader> shaders;
            shaders.push_back(gk3d::Shader::shaderFromFile(ResourcePath(vertexFilename), GL_VERTEX_SHADER));
            shaders.push_back(gk3d::Shader::shaderFromFile(ResourcePath(fragmentFilename), GL_FRAGMENT_SHADER));
            return new gk3d::Program(shaders);
        }

        // loads the content from file `filename` into gTexture
        static gk3d::Texture *LoadTexture(const char *filename) {
            gk3d::Bitmap bmp = gk3d::Bitmap::bitmapFromFile(ResourcePath(filename));
            bmp.flipVertically();
            return new gk3d::Texture(bmp);
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
        int currColor;

        ModelInstance() :
                asset(NULL),
                transform() {

            Light spotlight_exit;
            spotlight_exit.intensities = glm::vec3(0, 1, 0);
            spotlight_exit.position = glm::vec4(-70.0, 8.5, 0.0,1);
            spotlight_exit.attenuation = 0.1f;
            spotlight_exit.ambientCoefficient = 0.0f;
            spotlight_exit.coneAngle=360.0f;
            spotlight_exit.coneDirection=glm::vec3(-10,0,0);

            Light spotlight1;
            spotlight1.intensities = glm::vec3(1, 1, 0);
            spotlight1.position = glm::vec4(18.0, 9.5, -27.0,1);
            spotlight1.attenuation = 0.0f;
            spotlight1.ambientCoefficient = 0.0f;
            spotlight1.coneAngle=10.0f;
            spotlight1.coneDirection=glm::vec3(-1,-1,1);

            Light spotlight2;
            spotlight2.intensities = glm::vec3(1, 1, 0);
            spotlight2.position = glm::vec4(18.0, 9.5, 27.0,1);
            spotlight2.attenuation = 0.0f;
            spotlight2.ambientCoefficient = 0.0f;
            spotlight2.coneAngle=10.0f;
            spotlight2.coneDirection=glm::vec3(-1,-1,-1);

            Light spotlight3;
            spotlight3.intensities = glm::vec3(1, 0, 1);
            spotlight3.position = glm::vec4(-20.0, 9.5, -27.0,1);
            spotlight3.attenuation = 0.0f;
            spotlight3.ambientCoefficient = 0.0f;
            spotlight3.coneAngle=10.0f;
            spotlight3.coneDirection=glm::vec3(1,-1,1);

            Light spotlight4;
            spotlight4.intensities = glm::vec3(0, 1, 1);
            spotlight4.position = glm::vec4(-20.0, 9.5, 27.0,1);
            spotlight4.attenuation = 0.0f;
            spotlight4.ambientCoefficient = 0.0f;
            spotlight4.coneAngle=10.0f;
            spotlight4.coneDirection=glm::vec3(1,-1,-1);

            Light directional_light1;
            directional_light1.intensities = glm::vec3(0.1, 0.1, 0.1);
            directional_light1.position = glm::vec4(18.0, 9.5, 27.0,0);
            directional_light1.attenuation = 0.5f;
            directional_light1.ambientCoefficient = 0.0f;

            Light directional_light2;
            directional_light2.intensities = glm::vec3(0.1, 0.1, 0.1);
            directional_light2.position = glm::vec4(-20.0, 9.5, -27.0,0);
            directional_light2.attenuation = 0.1f;
            directional_light2.ambientCoefficient = 0.0f;

            Light directional_light3;
            directional_light3.intensities = glm::vec3(0.1, 0.1, 0.1);
            directional_light3.position = glm::vec4(-20.0, -9.5, -27.0,0);
            directional_light3.attenuation = 0.1f;
            directional_light3.ambientCoefficient = 0.0f;

            lights=std::vector<Light>();
            lights.push_back(spotlight_exit);
            lights.push_back(spotlight1);
            lights.push_back(spotlight2);
            lights.push_back(spotlight3);
            lights.push_back(spotlight4);
            lights.push_back(directional_light1);
            lights.push_back(directional_light2);
            lights.push_back(directional_light3);

            currColor=0;
        }

//        virtual void Render(const Camera &gCamera) const = 0;

        void Render(const Camera &gCamera, const RenderParams& params)  {

            gk3d::ModelAsset *asset = this->asset;
            for (int i = 0; i < asset->meshes.size(); ++i) {

                gk3d::Mesh *mesh = asset->meshes[i];
                gk3d::Program *shaders = mesh->shaders;

                //bind the shaders
                shaders->use();

                //set the shader uniforms
                shaders->setUniform("camera", gCamera.matrix());

                shaders->setUniform("model", this->transform);

                int t_size = (int) mesh->textures.size();

                //set the textures
                if (t_size > 0) {
                    shaders->setUniform("useTexture", 1.0f);
                    shaders->setUniform("numTextures", t_size);
                }

                for (int j = 0; j < t_size; ++j) {
                    glActiveTexture(GL_TEXTURE0 + j);
                    glBindTexture(GL_TEXTURE_2D, mesh->textures[j]->object());
                    SetUniform(shaders, "tex", NULL, j, j);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magTextureFilter);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minTextureFilter);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, params.bias);
                }

                shaders->setUniform("materialAmbientColor", mesh->ambientColor);
                shaders->setUniform("materialDiffuseColor", mesh->diffuseColor);
                shaders->setUniform("materialSpecularColor", mesh->specularColor);
                shaders->setUniform("materialShininess", mesh->shininess);

                shaders->setUniform("numLights", (int) lights.size());
                lights[1].intensities = currColor == 0 ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(1.f, 1.f, 1.f);
                currColor = (currColor + 1) % 2;
                for (size_t i = 0; i < lights.size(); ++i) {
                    SetUniform(shaders, "lights", "position", i, lights[i].position);
                    SetUniform(shaders, "lights", "intensities", i, lights[i].intensities);
                    SetUniform(shaders, "lights", "attenuation", i, lights[i].attenuation);
                    SetUniform(shaders, "lights", "ambientCoefficient", i, lights[i].ambientCoefficient);
                    SetUniform(shaders, "lights", "coneAngle", i, lights[i].coneAngle);
                    SetUniform(shaders, "lights", "coneDirection", i, lights[i].coneDirection);
                }

                if (params.fog != NULL) {
                    shaders->setUniform("fog.color", params.fog->color);
                    shaders->setUniform("fog.density", params.fog->density);
                    shaders->setUniform("fog.eq", params.fog->eq);
                    shaders->setUniform("fog.start", params.fog->start);
                    shaders->setUniform("fog.end", params.fog->end);
                }

                //bind VAO and draw
                glBindVertexArray(mesh->vao);
                glDrawArrays(mesh->drawType, mesh->drawStart, mesh->drawCount);

                //unbind everything
                glBindVertexArray(0);
                shaders->stopUsing();
            }
        }

        template <typename T>
        void SetUniform(gk3d::Program *shaders, const char *uniformName, const char *propertyName, size_t lightIndex, T &value) const{
            std::ostringstream ss;
            ss << uniformName << "[" << lightIndex << "]";
            if (propertyName != NULL) {
                ss << "." << propertyName;
            }
            std::string uniformName1 = ss.str();

            shaders->setUniform(uniformName1.c_str(), value);
        }

    };

}