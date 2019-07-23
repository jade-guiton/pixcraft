#pragma once

#include "glm.hpp"
#include "util.hpp"

class Player {
public:
	Player(glm::vec3 pos);
	
	glm::vec3 pos();
	void pos(glm::vec3 pos);
	void move(glm::vec3 dpos);
	
	glm::vec3 orient();
	void orient(glm::vec3 orient);
	void rotate(glm::vec3 dorient);
	
	glm::vec3 dirVector();
	Ray* castRay();
	
private:
	glm::vec3 _pos;
	glm::vec3 _orient;
};
