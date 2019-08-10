#pragma once

#include <cstdint>
#include <ctime>
#include <random>

#include "OpenSimplexNoise.hpp"
#include "wyhash.h"

class Random {
public:
	Random();
	Random(uint64_t seed);
	
	static uint64_t generateSeed();
	uint64_t getSeed();
	
	double getNoise(double x, double z);

private:
	uint64_t seed;
	OpenSimplexNoise noiseGen;
};
