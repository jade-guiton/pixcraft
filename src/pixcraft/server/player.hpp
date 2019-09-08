#pragma once

#include <tuple>
#include <memory>

#include "pixcraft/util/glm.hpp"

#include "world_module.hpp"
#include "mob.hpp"
#include "pixcraft/util/serializer_generated.h"

namespace PixCraft {
	enum class MovementMode { normal, flying, noClip };
	
	extern const char* movementModeNames[3];
	
	class Player : public Mob {
	public:
		Player(World& world, glm::vec3 pos);
		
		glm::vec3 eyePos();
		bool isEyeUnderwater();
		std::tuple<bool, int,int,int> castRay(float maxDist, bool offset, bool hitFluids);
		
		MovementMode movementMode();
		void movementMode(MovementMode mode);
		
		float getMaxHorSpeed();
		
		void handleKeys(std::tuple<int,int,bool,bool> mvtKeys, float dt);
		
		flatbuffers::Offset<void> serialize(flatbuffers::FlatBufferBuilder& builder) override;
		uint8_t serializedType() override;
		
		static std::unique_ptr<Player> unserialize(World& world, const Serializer::Player* playerData);
		
	private:
		MovementMode _movementMode;
	};
}
