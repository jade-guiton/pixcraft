#pragma once

#include <tuple>
#include <cmath>
#include <algorithm>
#include "glm.hpp"

#include "util.hpp"

class Player;
#include "world.hpp"

enum class MovementMode { normal, flying, noClip };

extern const char* movementModeNames[3];

class Player {
public:
	Player(World& world, glm::vec3 pos);
	
	glm::vec3 pos();
	glm::vec3 getFeetPos();
	void pos(glm::vec3 pos);
	glm::vec3 speed();
	
	glm::vec3 orient();
	void orient(glm::vec3 orient);
	void rotate(glm::vec3 dorient);
	
	glm::vec3 dirVector();
	std::tuple<bool, int,int,int> castRay(float maxDist, bool offset);
	
	bool canFly();
	bool collidesWithBlocks();
	float getMaxHorSpeed();
	
	void move(std::tuple<int,int,bool,bool> mvtKeys, float dt);
	
	MovementMode movementMode;
	
private:
	World* world;
	glm::vec3 _pos;
	glm::vec3 _orient;
	glm::vec3 _speed;
	bool onGround;
	
	static constexpr float EYE_HEIGHT = 1.6;
	static constexpr float HEIGHT = 1.7;
	static constexpr float RADIUS = 0.2;
	
	static constexpr float WALK_SPEED = 5.0f;
	static constexpr float FLY_SPEED = 10.0f;
	static constexpr float NOCLIP_SPEED = 30.0f;
	
	static constexpr float GRAVITY = 24.0f;
	static constexpr float JUMP_HEIGHT = 1.3f;
	static constexpr float JUMP_SPEED = sqrt(2*JUMP_HEIGHT*GRAVITY);
};
