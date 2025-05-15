#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "../render/texture_2d.hpp"

// STD
#include <array>
#include <map>

enum class BlockSides {
    BACK,
    FRONT,
    BOTTOM,
    TOP,
    LEFT,
    RIGHT,
};

// 0 -> BACK
typedef std::array<TextureCoords, 6> BlockTexture;

struct Block {
    enum BlockType : int {
        AIR = 0,
        GRASS,
        STONE,
        DIRT,
        SAND,
        WOOD,
        NUM_BLOCKS // Used to count the number of block types
    };

    static constexpr float BLOCK_SCALE = 10.0f;
    BlockType type;

    // Each block type will have a set of texture coordinates.
    static const std::map<BlockType, BlockTexture> blockTextureCoords;

    // Constructor to initialize block type to AIR by default
    Block() : type(Block::AIR) {}

    // Get texture coordinates for different block types.
    static BlockTexture getTextureCoords(BlockType type) {
        return blockTextureCoords.at(type);
    }

    static constexpr int textureWidth = 128;
    static constexpr int textureHeight = 128;

};


#endif
