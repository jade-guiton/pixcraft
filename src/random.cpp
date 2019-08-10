#include "random.hpp"

Random::Random() : Random(Random::generateSeed()) {}
Random::Random(uint64_t seed) : noiseGen(seed) {}

uint64_t Random::generateSeed() {
	uint64_t a = (uint64_t) time(nullptr);
	std::random_device rand;
	uint64_t b = ((uint64_t) rand()) << 32 | ((uint64_t) rand());
	return wyhash64(a, b);
}

uint64_t Random::getSeed() {
	return seed;
}

double Random::getNoise(double x, double z) {
	return noiseGen.Evaluate(x, z);
}
