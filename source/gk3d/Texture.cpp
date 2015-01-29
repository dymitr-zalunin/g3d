/*
 tdogl::Texture
 
 Copyright 2012 Thomas Dalling - http://tomdalling.com/
 
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

#include "Texture.h"
#include <stdexcept>

using namespace gk3d;

static GLenum TextureFormatForBitmapFormat(Bitmap::Format format) {
    switch (format) {
        case Bitmap::Format_Grayscale:
            return GL_LUMINANCE;
        case Bitmap::Format_GrayscaleAlpha:
            return GL_LUMINANCE_ALPHA;
        case Bitmap::Format_RGB:
            return GL_RGB;
        case Bitmap::Format_RGBA:
            return GL_RGBA;
        default:
            throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

Texture::Texture(const Bitmap &bitmap, GLint minMagFiler, GLint wrapMode) :
        _originalWidth((GLfloat) bitmap.width()),
        _originalHeight((GLfloat) bitmap.height()) {
    glGenTextures(1, &_object);
    glBindTexture(GL_TEXTURE_2D, _object);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagFiler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFiler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexImage2D(GL_TEXTURE_2D,
            0,
            TextureFormatForBitmapFormat(bitmap.format()),
            (GLsizei) bitmap.width(),
            (GLsizei) bitmap.height(),
            0,
            TextureFormatForBitmapFormat(bitmap.format()),
            GL_UNSIGNED_BYTE,
            bitmap.pixelBuffer());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture() :
        _originalWidth(64),
        _originalHeight(64) {
    GLuint CubeMapTarget[6];
    CubeMapTarget[0] = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
    CubeMapTarget[1] = GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
    CubeMapTarget[2] = GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
    CubeMapTarget[3] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
    CubeMapTarget[4] = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
    CubeMapTarget[5] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glGenTextures(1, &_object);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, _object);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTextureParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(CubeMapTarget[i],
                0,
                GL_RGB,
                64,
                64,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                NULL);
    }
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
}

Texture::~Texture() {
    glDeleteTextures(1, &_object);
}

GLuint Texture::object() const {
    return _object;
}

GLfloat Texture::originalWidth() const {
    return _originalWidth;
}

GLfloat Texture::originalHeight() const {
    return _originalHeight;
}