#pragma once

#include <cmath>
#include <string>
#include <cstdio>
#include <tuple>

#include "glm.hpp"

extern const int sideVectors[6][3];

#define TAU 6.28318531f

glm::mat4 globalToLocalRot(glm::vec3 orient);
glm::mat4 localToGlobalRot(glm::vec3 orient);
glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient);
glm::mat4 localToGlobal(glm::vec3 pos, glm::vec3 orient);

std::string vec3ToString(glm::vec3 pos);

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

class Ray {
public:
	Ray(glm::vec3 startPos, glm::vec3 dir);
	
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


std::tuple<int,int,int> getBlockCoordsAt(glm::vec3 pos);

// Tests [x1; x2] and [y1; y2] for intersection (in a narrow sense)
bool intervalIntersect(float x1, float x2, float y1, float y2);

int clampInt(int x, int min, int max);

glm::vec3 snapToEdge(glm::vec3 pos, float radius, int snapX, int snapY, int snapZ);
int collateCollisions(int a, int b, int c);
