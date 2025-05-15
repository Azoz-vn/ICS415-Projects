#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "block.hpp"

#include "../maths/vec.hpp"

#include <glad/glad.h>

// std
#include <vector>

class Chunk {
public:
    struct Vertex {
        Vec3f position;
        Vec3f texture; // The third element is for the ao occlusion.
        Vec3f color;
    };

    static constexpr int CHUNK_SIZE = 16;

    Chunk(int x, int y, int z);
    void destroy();

    void render();
    void reloadMesh();

    /* Gettets */
    Block &getBlock(int x, int y, int z) {
        int index = x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE);
        return blocks[index];
    }

    bool isBlockAtPosition(int x, int y, int z) {
        if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) return false;
        return getBlock(x, y, z).type == Block::AIR;
    }
    Vec3i getChunkPos() { return chunkPosition; }
private:
    GLuint vbo;
    GLuint vao;

    std::vector<Vertex> vertices {};
    std::vector<Block> blocks {CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, Block()};

    Vec3i chunkPosition;
};

#endif
