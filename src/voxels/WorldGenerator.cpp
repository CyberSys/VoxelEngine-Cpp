#include "WorldGenerator.hpp"

#include "content/Content.hpp"
#include "Block.hpp"
#include "Chunk.hpp"
#include "voxel.hpp"
#include "world/generator/GeneratorDef.hpp"

WorldGenerator::WorldGenerator(
    const GeneratorDef& def,
    const Content* content
) : def(def), content(content) {
}

void WorldGenerator::generate(voxel* voxels, int chunkX, int chunkZ, int seed) {
    auto heightmap = def.script->generateHeightmap(
        {chunkX*CHUNK_W, chunkZ*CHUNK_D}, {CHUNK_W, CHUNK_D}
    );
    auto& baseStone = content->blocks.require("base:stone");
    auto& baseWater = content->blocks.require("base:water");
    for (uint z = 0; z < CHUNK_D; z++) {
        for (uint x = 0; x < CHUNK_W; x++) {
            auto height = heightmap->getValues()[z * CHUNK_W + x] * 255 + 10;
            for (uint y = 0; y < CHUNK_H; y++) {
                voxels[vox_index(x, y, z)].state = {};
                if (y > height) {
                    voxels[vox_index(x, y, z)].id = y <= 64 ? baseWater.rt.id : 0;
                } else {
                    voxels[vox_index(x, y, z)].id = baseStone.rt.id;
                }
            }
        }
    }
}
