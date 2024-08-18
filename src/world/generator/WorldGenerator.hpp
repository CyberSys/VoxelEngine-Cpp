#pragma once

#include <string>

#include "typedefs.hpp"

struct voxel;
class Content;
struct GeneratorDef;

class WorldGenerator {
    const GeneratorDef& def;
    const Content* content;
public:
    WorldGenerator(
        const GeneratorDef& def,
        const Content* content
    );
    virtual ~WorldGenerator() = default;

    virtual void generate(voxel* voxels, int x, int z, uint64_t seed);

    inline static std::string DEFAULT = "core:default";
};
