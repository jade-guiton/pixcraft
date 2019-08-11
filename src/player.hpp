#pragma once

#include <tuple>
#include <utility>
#include <cmath>

#include "glm.hpp"

#include "world_module.hpp"

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
	std::tuple<bool, int,int,int> castRay(float maxDist, bool offset, bool hitFluids);
	
	bool canFly();
	bool collidesWithBlocks();
	float getMaxHorSpeed();
	
	std::pair<glm::vec3, glm::vec3> getBoundingBox();
	bool isInsideBlock(int32_t x, int32_t y, int32_t z);
	int getUnderwaterLevel();
	
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
	static constexpr float SWIM_SPEED = 5.0f;
	static constexpr float SWIM_ACCEL = 40.0f;
	
	static constexpr float GRAVITY = 32.0f;
	static constexpr float BUOYANCY = 20.0f;
	static constexpr float JUMP_HEIGHT = 1.3f;
	static constexpr float JUMP_SPEED = sqrt(2*JUMP_HEIGHT*GRAVITY);
};
