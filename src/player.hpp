#pragma once

#include <tuple>
#include <cmath>
#include <algorithm>
#include "glm.hpp"

#include "util.hpp"

class Player;
#include "world.hpp"

class Player {
public:
	Player(World& world, glm::vec3 pos);
	
	glm::vec3 pos();
	glm::vec3 getFeetPos();
	void pos(glm::vec3 pos);
	void move(glm::vec3 dpos);
	
	glm::vec3 orient();
	void orient(glm::vec3 orient);
	void rotate(glm::vec3 dorient);
	
	glm::vec3 dirVector();
	std::tuple<bool, int,int,int> castRay(float maxDist, bool offset);
	
	void collideOut();
	
	static constexpr float EYE_HEIGHT = 1.6;
	static constexpr float HEIGHT = 1.7;
	static constexpr float RADIUS = 0.2;
	
private:
	World* world;
	glm::vec3 _pos;
	glm::vec3 _orient;
};
