#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "../maths/vec.hpp"
#include <vector>
#include <glad/glad.h>

#include "../maths/mat4.hpp"

class Texture2D;
class ShaderProgram;

class Particle {
public:
    Vec3f position;
    Vec3f velocity;
    float lifetime; // How long the particle should live
    float size;     // Size of the particle
    bool hasGravity;

    Particle(const Vec3f& pos, const Vec3f& vel, float life, float size, bool gravity)
        : position(pos), velocity(vel), lifetime(life), size(size), hasGravity(gravity) {}

    void update(float deltaTime) {
        if (hasGravity) {
            velocity[1] -= 0.8f * deltaTime; // Lower gravity
        }

        position = position + (velocity * deltaTime); // Update position
        lifetime -= deltaTime;
        size -= deltaTime * 0.05f;
        if (size < 0.01f) size = 0.01f;
    }

    bool isExpired() const {
        return lifetime <= 0.0f || size <= 0.01f;
    }
};

class ParticleSystem {
public:
    std::vector<Particle> particles;

    void addParticle(const Vec3f& position, const Vec3f& velocity, float lifetime, float size, bool hasGravity) {
        particles.push_back(Particle(position, velocity, lifetime, size, hasGravity));
    }

    void update(float deltaTime) {
        for (auto it = particles.begin(); it != particles.end();) {
            it->update(deltaTime);
            if (it->isExpired()) {
                it = particles.erase(it); // Remove expired particles
            } else {
                ++it;
            }
        }
    }

    void render(ShaderProgram &particleShader, GLuint quadVAO, const Vec3f& cameraPosition) {
        // Build model matrices per particle
        std::vector<Mat4> modelMatrices;
        for (const auto& p : particles) {
            Mat4 model;

            // Calculate the direction from the particle to the camera
            Vec3f direction = cameraPosition - p.position;
            direction.normalize();

            // Compute a rotation matrix to face the camera
            float angleY = atan2(direction[0], direction[2]);  // Rotation around Y-axis
            float angleX = atan2(direction[1], sqrt(direction[0] * direction[0] + direction[2] * direction[2])); // Rotation around X-axis

            // Apply the rotation to the model matrix
            model = model * Mat4::rotateY(angleY);  // Rotate around Y
            model = model * Mat4::rotateX(angleX);  // Rotate around X

            model = model * Mat4::scale(p.size, p.size, p.size); // Apply scale
            model = model * Mat4::translation(p.position[0], p.position[1], p.position[2]); // Apply translation

            modelMatrices.push_back(model);
        }

        // Upload model matrices as instance data
        GLuint modelVBO;
        glGenBuffers(1, &modelVBO);
        glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
        glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(Mat4), modelMatrices.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(quadVAO);

        // Set up vertex attributes for mat4 (4 vec4 attributes)
        std::size_t vec4Size = sizeof(float) * 4;
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(2 + i);
            glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4), (void*)(i * vec4Size));
            glVertexAttribDivisor(2 + i, 1); // One matrix per instance
        }

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particles.size());

        // Cleanup
        glDeleteBuffers(1, &modelVBO);
    }

};

static inline GLuint createQuadVAO() {
    float quadVertices[] = {
        // positions         // texture coords
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,

        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // position
    glEnableVertexAttribArray(0);

    return quadVAO;
}

#endif
