#pragma once

#include "../glm.hpp"

#include "world_module.hpp"
#include "mob.hpp"

namespace PixCraft {
	class Slime : public Mob {
	public:
		Slime(World& world, glm::vec3 pos);
		
		void update(float dt) override;
		
		flatbuffers::Offset<void> serialize(flatbuffers::FlatBufferBuilder& builder) override;
		uint8_t serializedType() override;
	};
}
