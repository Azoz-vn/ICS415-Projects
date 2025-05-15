#include "player.hpp"

#include "world/particle.hpp"

Player::Player(Camera &camera, World &world, float moveSpeed) : camera(camera), moveSpeed(moveSpeed), world(world) {}

void Player::handleMouseInput(CrossPlatformWindow &window) const {
    // If needing to press middle mouse button is annoying, remove this if statment..
    if (window.getMouse().buttons[CrossPlatformWindow::MOUSE_MIDDLE]) {
        static float prevMouseX = 0.0f, prevMouseY = 0.0f;

        const auto mouseX = static_cast<float>(window.getMouse().x);
        const auto mouseY = static_cast<float>(window.getMouse().y);

        // Calculate the difference in mouse position (movement since the last frame).
        const auto deltaX = static_cast<float>(window.getMouse().deltaX);
        const auto deltaY = static_cast<float>(window.getMouse().deltaY);

        camera.rotate(-deltaY * sensitivity, deltaX * sensitivity);

        // Update the previous mouse position for the next frame.
        prevMouseX = mouseX;
        prevMouseY = mouseY;
    }
}

void Player::handleKeyboardInput(CrossPlatformWindow& window) {
    if (window.getKeyPresssed(KEY_VAL_W)) { camera.move(0, 0, moveSpeed); }
    if (window.getKeyPresssed(KEY_VAL_A)) { camera.move(-moveSpeed, 0, 0); }
    if (window.getKeyPresssed(KEY_VAL_S)) { camera.move(0, 0, -moveSpeed); }
    if (window.getKeyPresssed(KEY_VAL_D)) { camera.move(moveSpeed, 0, 0); }
    if (window.getKeyPresssed(KEY_VAL_T)) {
        placingBlockType = static_cast<Block::BlockType>((static_cast<int>(placingBlockType) + 1) % Block::NUM_BLOCKS);
        if (placingBlockType == Block::AIR) {
            placingBlockType = Block::GRASS;  // Or whatever default you want
        }
    }

    if (window.getKeyPresssed(KEY_VAL_SPACE)) { camera.move(0, moveSpeed, 0); }
    if (window.getKeyPresssed(KEY_VAL_LSHIFT)) { camera.move(0, -moveSpeed, 0); }
}

void Player::update(World &world, CrossPlatformWindow& window, ParticleSystem &particleSystem) {
    camera.updateViewMatrix();

    auto now = std::chrono::steady_clock::now();
    if (now - lastActionTime > actionDelay) {
        Vec3i collidedBlock, normal;

        Chunk *chunk = nullptr;

        // Try casting ray into adjacent chunks, increasing max distance if needed
        if (rayCast(&chunk, collidedBlock, normal, window, 10 * Block::BLOCK_SCALE)) {
            bool didAction = false;

            // Invalid chunk provided, exit function.
            if (chunk == nullptr) return;

            if (window.getMouse().buttons[CrossPlatformWindow::MOUSE_LEFT]) {

                // Break the block.
                chunk->getBlock(collidedBlock[0], collidedBlock[1], collidedBlock[2]).type = Block::AIR;
                didAction = true;

                const Vec3f particleOrigin = {
                    static_cast<float>(collidedBlock[0] + chunk->getChunkPos()[0] * Chunk::CHUNK_SIZE),
                    static_cast<float>(collidedBlock[1] + chunk->getChunkPos()[1] * Chunk::CHUNK_SIZE),
                    static_cast<float>(collidedBlock[2] + chunk->getChunkPos()[2] * Chunk::CHUNK_SIZE)
                };
                setParticlesOnBlockBreak(particleSystem, particleOrigin);
            }

            // Flip normal to get placement direction
            normal[0] = -normal[0];
            normal[1] = -normal[1];
            normal[2] = -normal[2];

            if (window.getMouse().buttons[CrossPlatformWindow::MOUSE_RIGHT]) {
                Vec3i placePos = collidedBlock + normal;
                chunk->getBlock(placePos[0], placePos[1], placePos[2]).type = placingBlockType;
                didAction = true;
            }

            if (didAction) {
                chunk->reloadMesh();  // Reload the mesh for the correct chunk
                lastActionTime = now;
            }
        }
    }
}

bool Player::rayCast(Chunk **hitChunk, Vec3i &hitPos, Vec3i &hitNormal, CrossPlatformWindow &window, const float maxDistance) const {
    const float blockScale = Block::BLOCK_SCALE;

    Vec3f origin = camera.getPosition();
    Vec3f forward = camera.getForward().normalize();
    Vec3f right = camera.getRight().normalize();
    Vec3f up = camera.getUp().normalize();

    up = up * -1.0f; // Flip for screen space

    float mouseX = static_cast<float>(window.getMouse().x);
    float mouseY = static_cast<float>(window.getMouse().y);

    float normalizedX = (mouseX / window.getWidth()) * 2.0f - 1.0f;
    float normalizedY = (mouseY / window.getHeight()) * 2.0f - 1.0f;

    float sensitivity = 0.5f;
    Vec3f direction = (forward + right * normalizedX * sensitivity + up * normalizedY * sensitivity).normalize();

    const float step = 0.05f;
    Vec3f pos = origin;
    Vec3f prevPos = origin;

    for (float t = 0.0f; t <= maxDistance; t += step) {
        pos = origin + direction * t;

        int blockX = static_cast<int>(floor(pos[0] / blockScale));
        int blockY = static_cast<int>(floor(pos[1] / blockScale));
        int blockZ = static_cast<int>(floor(pos[2] / blockScale));

        // Compute chunk position
        int chunkX = blockX / Chunk::CHUNK_SIZE;
        int chunkZ = blockZ / Chunk::CHUNK_SIZE;

        // Adjust for negative coordinates
        if (blockX < 0 && blockX % Chunk::CHUNK_SIZE != 0) chunkX--;
        if (blockZ < 0 && blockZ % Chunk::CHUNK_SIZE != 0) chunkZ--;

        Chunk *chunk = world.getChunk(World::chunkKey(chunkX, chunkZ));
        if (!chunk) continue;

        // Local block coords
        int localX = blockX % Chunk::CHUNK_SIZE;
        int localY = blockY % Chunk::CHUNK_SIZE;
        int localZ = blockZ % Chunk::CHUNK_SIZE;
        if (localX < 0) localX += Chunk::CHUNK_SIZE;
        if (localY < 0) localY += Chunk::CHUNK_SIZE;
        if (localZ < 0) localZ += Chunk::CHUNK_SIZE;

        Block block = chunk->getBlock(localX, localY, localZ);
        if (block.type != Block::AIR) {
            hitPos = { localX, localY, localZ };

            // Normal = difference between current and previous voxel
            int prevX = static_cast<int>(floor(prevPos[0] / blockScale));
            int prevY = static_cast<int>(floor(prevPos[1] / blockScale));
            int prevZ = static_cast<int>(floor(prevPos[2] / blockScale));

            hitNormal = { blockX - prevX, blockY - prevY, blockZ - prevZ };

            if (hitChunk) {
                *hitChunk = chunk;
            }

            return true;
        }

        prevPos = pos;
    }

    return false;
}


void Player::setParticlesOnBlockBreak(ParticleSystem &particleSystem, const Vec3f &origin) {
    /*
     * An attemp to make the particles go up with velocity (a sort of burst).
     * When breaking a block.
     */
    constexpr int numParticles = 20;
    for (int i = 0; i < numParticles; ++i) {
        // Random offset from the center of the block (0.0 to 1.0).
        Vec3f offset = {
            (static_cast<float>(rand()) / RAND_MAX),
            (static_cast<float>(rand()) / RAND_MAX),
            (static_cast<float>(rand()) / RAND_MAX)
        };

        constexpr float speed = 1.80f;

        // Random direction vector (normalized) with upward bias
        Vec3f dir = {
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f),
            ((static_cast<float>(rand()) / RAND_MAX) * 0.5f + 0.5f), // upward bias
            ((static_cast<float>(rand()) / RAND_MAX) - 0.5f)
        };

        // Normalize the direction vector (assuming you have a .normalized() method)
        dir = dir.normalize();

        Vec3f velocity = dir * speed;

        // Slightly randomize size and lifetime
        const float size = (Block::BLOCK_SCALE / 10.0f) + float(rand()) / RAND_MAX;
        const float lifetime = 8.5f + float(rand()) / RAND_MAX * 1.0f;

        // Has to obied by gravity to get the minecrafty explosion.
        particleSystem.addParticle((origin + offset) * Block::BLOCK_SCALE, velocity, lifetime, size, true);
    }
}