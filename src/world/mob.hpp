#pragma once

#include <utility>

#include "../glm.hpp"

#include "world_module.hpp"

namespace PixCraft {
	constexpr float GRAVITY = 32.0f;
	
	class Mob {
	public:
		glm::vec3 pos();
		void pos(glm::vec3 pos);
		glm::vec3 speed();
		
		glm::vec3 orient();
		void orient(glm::vec3 orient);
		void rotate(glm::vec3 dorient);
		
		glm::vec3 dirVector();
		
		std::pair<glm::vec3, glm::vec3> getBoundingBox();
		bool isInsideBlock(int32_t x, int32_t y, int32_t z);
		float getWaterHeight();
		
		virtual void update(float dt);
		
	protected:
		World& world;
		
		float height;
		float radius;
		
		bool canFly;
		bool collidesWithBlocks;
		bool onGround;
		glm::vec3 _pos;
		glm::vec3 _orient;
		glm::vec3 _speed;
		
		Mob(World& world, float height, float radius, bool canFly, bool collidesWithBlocks, glm::vec3 pos, glm::vec3 orient);
	};
}
