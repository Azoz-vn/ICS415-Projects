#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "camera.hpp"

#include "maths/vec.hpp"
#include "world/world.hpp"
#include "window/cross_platform_window.hpp"

#include <chrono>

class ParticleSystem;

class Player {
public:
    Player(Camera &camera, World &world, float moveSpeed);

    void handleMouseInput(CrossPlatformWindow &window) const;
    void handleKeyboardInput(CrossPlatformWindow &window);
    void update(World &world, CrossPlatformWindow &window, ParticleSystem &particleSystem);

    bool rayCast(Chunk **hitChunk, Vec3i &hitPos, Vec3i &hitNormal, CrossPlatformWindow &window, float maxDistance) const;

    static void setParticlesOnBlockBreak(ParticleSystem &particleSystem, const Vec3f &origin);


    // What block will be placed when the player clicks right clidck.
    Block::BlockType placingBlockType = Block::STONE;
private:
    // MOVEMENT.
    Camera &camera;
    float moveSpeed = 0.0f;
    static constexpr float sensitivity = 0.08f;

    World &world;

    // Time tracking.
    std::chrono::steady_clock::time_point lastActionTime;
    std::chrono::milliseconds actionDelay = std::chrono::milliseconds(200); // 200 ms delay
};

#endif // PLAYER_HPP
