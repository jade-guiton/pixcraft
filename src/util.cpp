#include "util.hpp"

const int sideVectors[6][3] = {
	{0,0,1}, {1,0,0}, {0,0,-1}, {-1,0,0}, {0,-1,0}, {0,1,0}
};

glm::mat4 globalToLocalRot(glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, -orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::rotate(mat, -orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, -orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	return mat;
}

glm::mat4 localToGlobalRot(glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return mat;
}

glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = globalToLocalRot(orient);
	mat = glm::translate(mat, -pos);
	return mat;
}

glm::mat4 localToGlobal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, pos);
	mat = glm::rotate(mat, orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	return mat;
}


SpiralIterator::SpiralIterator(int startX, int startZ)
	: startX(startX), startZ(startZ), x(0), z(0), dx(1), dz(0) { }

int SpiralIterator::getX() { return startX + x; }
int SpiralIterator::getZ() { return startZ + z; }

bool SpiralIterator::withinDistance(int dist) {
	return abs(x) <= dist && abs(z) <= dist;
}

void SpiralIterator::next() {
	x += dx;
	z += dz;
	if(x == z || (x < 0 && x == -z) || (x > 0 && x == 1-z)) {
		int dz0 = dz;
		dz = dx;
		dx = -dz0;
	}
}


int sign(float x) {
	if(x > 0) return 1;
	else if(x < 0) return -1;
	else return 0;
}

int getBlockCoordAt(float x) {
	return (int) round(x);
}

Raycast::Raycast(glm::vec3 startPos, glm::vec3 dir)
	: tDeltaX(1 / abs(dir.x)), tDeltaY(1 / abs(dir.y)), tDeltaZ(1 / abs(dir.z)),
	  stepX(sign(dir.x)), stepY(sign(dir.y)), stepZ(sign(dir.z)),
	  x(getBlockCoordAt(startPos.x)), y(getBlockCoordAt(startPos.y)), z(getBlockCoordAt(startPos.z)),
	  dist(0.0f), lastFace(0) {
	if(dir.x > 0) {
		tMaxX = (x + 0.5 - startPos.x) / dir.x;
	} else {
		tMaxX = (x - 0.5 - startPos.x) / dir.x;
	}
	if(dir.y > 0) {
		tMaxY = (y + 0.5 - startPos.y) / dir.y;
	} else {
		tMaxY = (y - 0.5 - startPos.y) / dir.y;
	}
	if(dir.z > 0) {
		tMaxZ = (z + 0.5 - startPos.z) / dir.z;
	} else {
		tMaxZ = (z - 0.5 - startPos.z) / dir.z;
	}
}

int Raycast::getX() { return x; }
int Raycast::getY() { return y; }
int Raycast::getZ() { return z; }
float Raycast::getDistance() { return dist; }
int Raycast::getLastFace() { return lastFace; }

void Raycast::nextFace() {
	if(tMaxX < tMaxY && tMaxX < tMaxZ) { // next face on X axis
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
