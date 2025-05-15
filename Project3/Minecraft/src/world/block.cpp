#include "block.hpp"

/*
 * Reminder that opengl flips the texture so 0, 0 is in the bottom right and the top left is the max, max.
 */
const std::map<Block::BlockType, BlockTexture> Block::blockTextureCoords = {
    { Block::GRASS, BlockTexture{
            0, 96, 32, 32,
            0, 96, 32, 32,
            32, 96, 32, 32,
            32, 96, 32, 32,
            0, 96, 32, 32,
            0, 96, 32, 32,
        }
    },

    { Block::DIRT, BlockTexture{
            0, 64, 32, 32,
            0, 64, 32, 32,
            0, 64, 32, 32,
            0, 64, 32, 32,
            0, 64, 32, 32,
            0, 64, 32, 32
        }
    },

    { Block::STONE, BlockTexture{
            0, 32, 32, 32,
            0, 32, 32, 32,
            0, 32, 32, 32,
            0, 32, 32, 32,
            0, 32, 32, 32,
            0, 32, 32, 32
        }
    },

    { Block::SAND, BlockTexture{
            0, 0, 32, 32,
            0, 0, 32, 32,
            0, 0, 32, 32,
            0, 0, 32, 32,
            0, 0, 32, 32,
            0, 0, 32, 32
        }
    },

    { Block::WOOD, BlockTexture{
            32, 0, 32, 32,
            32, 0, 32, 32,
            32, 0, 32, 32,
            32, 0, 32, 32,
            32, 0, 32, 32,
            32, 0, 32, 32
        }
    },
};
