#include "util.hpp"

glm::mat4 globalToLocal(glm::vec3 pos, glm::vec3 orient) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, -orient.z, glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::rotate(mat, -orient.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat = glm::rotate(mat, -orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::translate(mat, -pos);
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
