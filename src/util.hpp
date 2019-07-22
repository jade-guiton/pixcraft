#pragma once

#include <cmath>

#include "glm.hpp"

extern const int sideVectors[6][3];

#define TAU 6.28318531f

glm::mat4 globalToLocalRot(glm::vec3 orient);
glm::mat4 localToGlobalRot(glm::vec3 orient);
glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient);
glm::mat4 localToGlobal(glm::vec3 pos, glm::vec3 orient);

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


int sign(float x);

// From a world coordinate, returns the block it's inside's corresponding coordinate
int getBlockCoordAt(float x);

class Raycast {
public:
	Raycast(glm::vec3 startPos, glm::vec3 dir);
	
	int getX();
	int getY();
	int getZ();
	float getDistance();
	int getLastFace();
	
	void nextFace();

private:
	float tDeltaX, tDeltaY, tDeltaZ;
	int stepX, stepY, stepZ;
	
	int x, y, z;
	float dist;
	float tMaxX, tMaxY, tMaxZ;
	int lastFace;
};
