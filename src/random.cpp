#include "random.hpp"

uint64_t generateSeed() {
	uint64_t a = static_cast<uint64_t>(time(nullptr));
	std::random_device rand;
	uint64_t b = ((uint64_t) rand()) << 32 | ((uint64_t) rand());
	return wyhash64(a, b);
}

uint64_t getFeatureSeed(uint64_t seed, FeatureType feature) {
	return wyhash64(seed, static_cast<uint64_t>(feature));
}

uint64_t randFromPosition(uint64_t seed, int32_t x, int32_t z) {
	return wyhash64(seed, packCoords(x, z));
}

std::vector<float> distributeObjects(uint64_t seed, float minX, float minZ, float chunkSize, float minDist, float size) {
	int32_t minX2 = minX - size;
	int32_t minZ2 = minZ - size;
	int32_t maxX2 = minX + chunkSize + size;
	int32_t maxZ2 = minZ + chunkSize + size;
	
	float tileSize = minDist / 0.1f; // 0.1f == radius of original distribution
	
	int32_t minX3 = (int32_t) floor(minX2 / tileSize);
	int32_t minZ3 = (int32_t) floor(minZ2 / tileSize);
	int32_t maxX3 = (int32_t) ceil(maxX2 / tileSize);
	int32_t maxZ3 = (int32_t) ceil(maxZ2 / tileSize);
	
	std::vector<float> objects;
	for(int32_t x = minX3; x < maxX3; ++x) {
		for(int32_t z = minZ3; z < maxZ3; ++z) {
			uint8_t c1 = randFromPosition(seed, x, z) % 4;
			uint8_t c2 = randFromPosition(seed, x+1, z) % 4;
			uint8_t c3 = randFromPosition(seed, x, z+1) % 4;
			uint8_t c4 = randFromPosition(seed, x+1, z+1) % 4;
			std::vector<float> tile = poissonDiskTiles[(c1 << 6) | (c2 << 4) | (c3 << 2) | c4];
			for(size_t i = 0; i < tile.size(); i += 2) {
				float pointX = (x + tile[i]) * tileSize;
				float pointZ = (z + tile[i + 1]) * tileSize;
				if(pointX >= minX2 && pointX <= maxX2
					&& pointZ >= minZ2 && pointZ <= maxZ2) {
					objects.push_back(pointX);
					objects.push_back(pointZ);
				}
			}
		}
	}
	
	return objects;
}
