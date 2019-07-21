#pragma once

#include <cmath>

#include "glm.hpp"

glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient);

class SpiralIterator {
public:
	SpiralIterator(int startX, int startZ);
	
	int getX();
	int getZ();
	bool withinDistance(int dist);
	
	void next();
	
private:
	int startX, startZ;
	int x, z;
	int dx, dz;
};
