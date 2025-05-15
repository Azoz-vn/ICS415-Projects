#include "chunk.hpp"

Chunk::Chunk(int x, int y, int z) : chunkPosition(x, y, z) {
    // Generate terrain
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                int index = x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_SIZE);
                blocks[index].type = (y == 7) ? Block::GRASS : (y < 7 && y >= 5) ? Block::DIRT : (y < 5) ? Block::STONE : Block::AIR;
            }
        }
    }

    // generate vertices

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    reloadMesh();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texture));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color));
}

void Chunk::destroy() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Chunk::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void Chunk::reloadMesh() {
    vertices.clear();

    auto isBlockVisible = [&](int x, int y, int z) {
        if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE)
            return true; // Face on edge of chunk.

        int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
        return blocks[index].type == Block::AIR;
    };


    auto addFace = [&](int x, int y, int z, int face, Block::BlockType type) {
        // Get texture coordinates and dimensions for the block type
        BlockTexture textureCoords = Block::getTextureCoords(type);

        // Corrected texture vertices for each face
        static const float faceVertices[6][6][3] = {
            // -Z (back)
            {{0,0,0}, {1,0,0}, {0,1,0}, {0,1,0}, {1,0,0}, {1,1,0}},
            // +Z (front)
            {{0,0,1}, {0,1,1}, {1,0,1}, {1,0,1}, {0,1,1}, {1,1,1}},

            // -Y (bottom)
            {{0,0,0}, {0,0,1}, {1,0,0}, {1,0,0}, {0,0,1}, {1,0,1}},
            // +Y (top)
            {{0,1,0}, {1,1,0}, {0,1,1}, {0,1,1}, {1,1,0}, {1,1,1}},

            // -X (left)
            {{0,0,0}, {0,1,0}, {0,0,1}, {0,0,1}, {0,1,0}, {0,1,1}},
            // +X (right)
            {{1,0,0}, {1,0,1}, {1,1,0}, {1,1,0}, {1,0,1}, {1,1,1}},
        };

        static const float textureVertices[6][6][2] = {
            // -Z (back)
            {{0,0}, {1,0}, {0,1}, {0,1}, {1,0}, {1,1}},

            // +Z (front) - flipped horizontally to correct orientation
            {{1,0}, {1,1}, {0,0}, {0,0}, {1,1}, {0,1}},

            // -Y (bottom)
            {{0,1}, {0,0}, {1,1}, {1,1}, {0,0}, {1,0}},

            // +Y (top)
            {{0,0}, {1,0}, {0,1}, {0,1}, {1,0}, {1,1}},

            // -X (left) - rotated to fix flipped texture
            {{1,0}, {1,1}, {0,0}, {0,0}, {1,1}, {0,1}},

            // +X (right) — reverted to original UVs
            {{0, 0}, {1, 0}, {0, 1}, {0, 1}, {1, 0}, {1, 1}},
        };

        static const float faceBrightness[6] = {
            0.3f, // -Z (back) — much darker
            1.0f, // +Z (front) — unchanged
            0.2f, // -Y (bottom) — very dark
            1.2f, // +Y (top) — unchanged
            0.4f, // -X (left) — darker
            0.85f // +X (right) — slightly darker
        };
        for (int i = 0; i < 6; i++) {
            Vertex v;

            // Texture mapping (unchanged)
            float u = (textureCoords[face].x + textureVertices[face][i][0] * textureCoords[face].width) / Block::textureWidth;
            float vCoord = (textureCoords[face].y + textureVertices[face][i][1] * textureCoords[face].height) / Block::textureHeight;

            v.texture[0] = u;
            v.texture[1] = vCoord;

            // World-space position
            v.position[0] = (x + faceVertices[face][i][0] + chunkPosition[0] * CHUNK_SIZE) * Block::BLOCK_SCALE;
            v.position[1] = (y + faceVertices[face][i][1] + chunkPosition[1] * CHUNK_SIZE) * Block::BLOCK_SCALE;
            v.position[2] = (z + faceVertices[face][i][2] + chunkPosition[2] * CHUNK_SIZE) * Block::BLOCK_SCALE;

            // Assign brightness as grayscale color
            float brightness = faceBrightness[face];
            v.color[0] = brightness;
            v.color[1] = brightness;
            v.color[2] = brightness;

            vertices.push_back(v);
        }

    };

    // Loop through all blocks and add faces based on visibility
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
                Block &block = blocks[index];
                if (block.type == Block::AIR)
                    continue;

                // Check neighbors and add visible faces.
                if (isBlockVisible(x, y, z - 1)) addFace(x, y, z, 0, block.type);
                if (isBlockVisible(x, y, z + 1)) addFace(x, y, z, 1, block.type);
                if (isBlockVisible(x, y - 1, z)) addFace(x, y, z, 2, block.type);
                if (isBlockVisible(x, y + 1, z)) addFace(x, y, z, 3, block.type);
                if (isBlockVisible(x - 1, y, z)) addFace(x, y, z, 4, block.type);
                if (isBlockVisible(x + 1, y, z)) addFace(x, y, z, 5, block.type);
            }
        }
    }

    // Upload vertex data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}
