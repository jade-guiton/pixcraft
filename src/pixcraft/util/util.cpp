#include "pixcraft/util/util.hpp"

#include <cmath>
#include <cstdio>
#include <limits>
#include <stdexcept>

using namespace PixCraft;

const int PixCraft::sideVectors[6][3] = {
	{0,0,1}, {1,0,0}, {0,0,-1}, {-1,0,0}, {0,-1,0}, {0,1,0}
};

glm::mat4 PixCraft::globalToLocalRot(glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, -orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::rotate(mat, -orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, -orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	return mat;
}

glm::mat4 PixCraft::localToGlobalRot(glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return mat;
}

glm::mat4 PixCraft::globalToLocal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = globalToLocalRot(orient);
	mat = glm::translate(mat, -pos);
	return mat;
}

glm::mat4 PixCraft::localToGlobal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, pos);
	mat = glm::rotate(mat, orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return mat;
}


std::string PixCraft::vec3ToString(glm::vec3 pos) {
	const size_t bufSize = 64;
	char buffer[bufSize];
	snprintf(buffer, bufSize, "(%7.2f; %7.2f; %7.2f)", pos.x, pos.y, pos.z);
	return std::string(buffer);
}


PixCraft::SpiralIterator::SpiralIterator(int startX, int startZ)
	: startX(startX), startZ(startZ), x(0), z(0), dx(1), dz(0) { }

int PixCraft::SpiralIterator::getX() { return startX + x; }
int PixCraft::SpiralIterator::getZ() { return startZ + z; }

bool PixCraft::SpiralIterator::withinSquareDistance(int dist) {
	return abs(x) <= dist && abs(z) <= dist;
}

bool PixCraft::SpiralIterator::withinDistance(int dist) {
	return x*x + z*z <= dist*dist;
}

void PixCraft::SpiralIterator::next() {
	x += dx;
	z += dz;
	if(x == z || (x < 0 && x == -z) || (x > 0 && x == 1-z)) {
		int dz0 = dz;
		dz = dx;
		dx = -dz0;
	}
}


int PixCraft::sign(float x) {
	if(x > 0) return 1;
	else if(x < 0) return -1;
	else return 0;
}

int PixCraft::getBlockCoordAt(float x) {
	return (int) round(x);
}

PixCraft::Ray::Ray(glm::vec3 startPos, glm::vec3 dir)
	: tDeltaX(1/fabs(dir.x)), tDeltaY(1/fabs(dir.y)), tDeltaZ(1/fabs(dir.z)),
	  stepX(sign(dir.x)), stepY(sign(dir.y)), stepZ(sign(dir.z)),
	  x(getBlockCoordAt(startPos.x)), y(getBlockCoordAt(startPos.y)), z(getBlockCoordAt(startPos.z)),
	  dist(0.0f), lastFace(0) {
	
	if(dir.x == 0 && dir.y == 0 && dir.z == 0)
		throw std::runtime_error("Trying to cast a ray with null direction vector");
	
	if(dir.x == 0) { // You will never reach the next X face. This is the power of parallel lines Requiem.
		tMaxX = std::numeric_limits<float>::infinity();
	} else if(dir.x > 0) {
		tMaxX = (x + 0.5 - startPos.x) / dir.x;
	} else {
		tMaxX = (x - 0.5 - startPos.x) / dir.x;
	}
	
	if(dir.y == 0) {
		tMaxY = std::numeric_limits<float>::infinity();
	} else if(dir.y > 0) {
		tMaxY = (y + 0.5 - startPos.y) / dir.y;
	} else {
		tMaxY = (y - 0.5 - startPos.y) / dir.y;
	}
	
	if(dir.z == 0) {
		tMaxZ = std::numeric_limits<float>::infinity();
	} else if(dir.z > 0) {
		tMaxZ = (z + 0.5 - startPos.z) / dir.z;
	} else {
		tMaxZ = (z - 0.5 - startPos.z) / dir.z;
	}
}

int PixCraft::Ray::getX() { return x; }
int PixCraft::Ray::getY() { return y; }
int PixCraft::Ray::getZ() { return z; }
float PixCraft::Ray::getDistance() { return dist; }
int PixCraft::Ray::getLastFace() { return lastFace; }

void PixCraft::Ray::nextFace() {
	if(tMaxX < tMaxY && tMaxX < tMaxZ) { // next face on X axis
		dist = tMaxX;
		tMaxX += tDeltaX;
		x += stepX;
		lastFace = 2 + stepX; // west or east faces
	} else if(tMaxY < tMaxX && tMaxY < tMaxZ) { // next face on Y axis
		dist = tMaxY;
		tMaxY += tDeltaY;
		y += stepY;
		lastFace = 4 + (1-stepY)/2; // bottom or top faces
	} else { // next face on Z axis
		dist = tMaxZ;
		tMaxZ += tDeltaZ;
		z += stepZ;
		lastFace = 1 + stepZ;
	}
}


std::tuple<int,int,int> PixCraft::getBlockCoordsAt(glm::vec3 pos) {
	return std::tuple<int,int,int>(getBlockCoordAt(pos.x), getBlockCoordAt(pos.y), getBlockCoordAt(pos.z));
}

uint64_t PixCraft::packCoords(int32_t x, int32_t z) {
	uint32_t x2 = (uint32_t) x;
	uint32_t z2 = (uint32_t) z;
	return ((uint64_t) x2) << 32 | ((uint64_t) z2);
}

std::pair<int32_t, int32_t> PixCraft::unpackCoords(uint64_t v) {
	int32_t x = (int32_t) ((uint32_t) (v >> 32));
	int32_t z = (int32_t) ((uint32_t) v);
	return std::pair<int32_t, int32_t>(x, z);
}

bool PixCraft::cylinderBlockCollision(glm::vec3 center, float radius, float height, int32_t x, int32_t y, int32_t z) {
	float relY = center.y - y;
	if(relY > 0.5 || relY < -0.5 - height) return false;
	float relX = center.x - x;
	if(relX < -0.5 - radius || relX > 0.5 + radius) return false;
	float relZ = center.z - z;
	if(relZ < -0.5 - radius || relZ > 0.5 + radius) return false;
	if((-0.5 <= relX && relX <= 0.5) || (-0.5 <= relZ && relZ <= 0.5)) return true;
	// if we reach here, we are in one of the corners and need to check.
	float cornerX = (relX >= 0) - 0.5f;
	float cornerZ = (relZ >= 0) - 0.5f;
	float dist2 = (relX - cornerX)*(relX - cornerX) + (relZ - cornerZ)*(relZ - cornerZ);
	return dist2 <= radius*radius;
}

glm::vec3 PixCraft::hslToRgb(glm::vec3 hsl) {
	float c = (1 - abs(2*hsl.z - 1)) * hsl.y;
	float h2 = hsl.x * 6;
	float x = c*(1 - abs(fmod(h2, 2) - 1));
	float r1 = 0, g1 = 0, b1 = 0;
	if(h2 <= 1)      { r1 = c; g1 = x; }
	else if(h2 <= 2) { r1 = x; g1 = c; }
	else if(h2 <= 3) { g1 = c; b1 = x; }
	else if(h2 <= 4) { g1 = x; b1 = c; }
	else if(h2 <= 5) { r1 = x; b1 = c; }
	else { r1 = c; b1 = x; }
	float m = hsl.z - c/2;
	return glm::vec3(r1 + m, g1 + m, b1 + m);
}
