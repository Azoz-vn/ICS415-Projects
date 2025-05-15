#include "world.hpp"

World::World(int chunkLoadRadius, int worldSize)
    : chunkLoadRadius(chunkLoadRadius), worldSize(worldSize) {


}

World::~World() {
    for (auto chunk : chunks) {
        chunk.second.destroy();
    }
}

void World::initChunks() {
    loadAllChunks();
}

void World::render(const Vec3f& playerPosition) {
    // Render all loaded chunks.
    for (auto& [key, chunk] : chunks) {
        chunk.render();
    }
}

std::string World::chunkKey(int x, int z) {
    return std::to_string(x) + "_" + std::to_string(z); // Chunk key: "x_z"
}

Chunk *World::getChunk(const std::string &key) {
    if (chunks.find(key) == nullptr) return nullptr;
    return &chunks.at(key);
}

void World::loadAllChunks() {
    // Generate all chunks within the fixed world size
    for (int x = 0; x < worldSize; ++x) {
        for (int z = 0; z < worldSize; ++z) {
            loadChunk(x, z);
        }
    }
}

void World::loadChunk(int x, int z) {
    std::string key = chunkKey(x, z);

    // If the chunk is not loaded, generate and load it.
    if (chunks.find(key) == chunks.end()) {
        chunks.emplace(key, Chunk(x, 0, z));
    }
}

void World::clearAllChunks() {
    chunks.clear();
}
