#ifndef WORLD_HPP
#define WORLD_HPP

#include "chunk.hpp"
#include "../maths/vec.hpp"

// STD
#include <unordered_map>
#include <memory>
#include <string>

class World {
public:
    World(int chunkLoadRadius, int worldSize);
    ~World();

    void initChunks();

    // Render all chunks in the world.
    void render(const Vec3f& playerPosition);

    /* Getters */
    static std::string chunkKey(int x, int z);
    Chunk *getChunk(const std::string &key);
    auto getChunks() { return chunks; }
private:
    std::unordered_map<std::string, Chunk> chunks; // Maps chunk coordinates to Chunk.
    int chunkLoadRadius; // Radius of chunks to consider for rendering
    int worldSize; // Size of the world in terms of chunks (fixed)

    void loadChunk(int x, int z);

    void loadAllChunks();

    // Removes all chunks (optional in case you want to clear or reset the world).
    void clearAllChunks();
};

#endif
