#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

// DEPEND
#include <glad/glad.h>
#include <stb/stb_image.h>

// STD
#include <string>
#include <iostream>

struct TextureCoords {
    int x, y;           // Starting position of the texture in the atlas
    int width, height;  // Width and height of the texture
};

class Texture2D {
public:
    GLuint id;
    int width, height, channels;

    Texture2D(const std::string& filePath) {
        glGenTextures(1, &id);

        // Load image using stb_image.
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        if (data) {
            // Generate OpenGL texture
            glBindTexture(GL_TEXTURE_2D, id); // Bind the texture
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrap S (horizontal).
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrap T (vertical).

            // Upload the texture to OpenGL.
            GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Free the image data after it's been uploaded to OpenGL.
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << filePath << std::endl;
            stbi_image_free(data);
        }
    }

    void bind(GLuint textureUnit = 0) const {
        glActiveTexture(GL_TEXTURE0 + textureUnit);     // Activate the correct texture unit
        glBindTexture(GL_TEXTURE_2D, id);    // Bind the texture
    }

    ~Texture2D() {
        glDeleteTextures(1, &id);
    }
};

#endif
