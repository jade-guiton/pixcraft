#pragma once

#include "../glm.hpp"

#include "world_module.hpp"
#include "mob.hpp"

class Slime : public Mob {
public:
	Slime(World& world, glm::vec3 pos);
	
	void update(float dt) override;
};
