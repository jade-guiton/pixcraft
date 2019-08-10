#pragma once

#include <cstdint>
#include <vector>

#include "wyhash.h"

#include "util.hpp"

extern const std::vector<float> poissonDiskTiles[256];

uint64_t generateSeed();

enum class FeatureType {
	terrainHeight,
	trees
};

uint64_t getFeatureSeed(uint64_t seed, FeatureType feature);

uint64_t randFromPosition(uint64_t seed, int32_t x, int32_t z);

// Samples a Poisson-disk distribution with radius minDist, returning the interleaved coordinates of a number of points,
// such that objects with a given size placed at these points overlap the square with corner (minX, minZ) and size chunkSize.
std::vector<float> distributeObjects(uint64_t seed, float minX, float minZ, float chunkSize, float minDist, float size);
