#include "slime.hpp"

#include <cmath>

#include "../util.hpp"

using namespace PixCraft;

const float JUMP_HEIGHT = 1.1f;
const float JUMP_SPEED = sqrt(2*JUMP_HEIGHT*GRAVITY);

const float HEIGHT = 0.6;
const float RADIUS = HEIGHT / sqrt(TAU / 2);

const float SPEED = 3.0f;

Slime::Slime(World& world, glm::vec3 pos)
	: Mob(world, HEIGHT, RADIUS, false, true, pos, glm::vec3(0.0)) {}

void Slime::update(float dt) {
	if(onGround) {
		_speed.y = JUMP_SPEED;
	} else {
		glm::mat4 yRot = glm::rotate(glm::mat4(1.0f), _orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 horSpeed = glm::vec3(yRot * glm::vec4(0, 0, -SPEED, 1.0f));
		_speed.x = horSpeed.x; _speed.z = horSpeed.z;
	}
	Mob::update(dt);
}

flatbuffers::Offset<void> Slime::serialize(flatbuffers::FlatBufferBuilder& builder) {
	auto mobBase = serializeMobBase(builder);
	return Serializer::CreateSlime(builder, mobBase).Union();
}

uint8_t Slime::serializedType() {
	return Serializer::Mob_Slime;
}

std::unique_ptr<Slime> Slime::unserialize(World& world, const Serializer::Slime* slimeData) {
	std::unique_ptr<Slime> slime(new Slime(world, glm::vec3(0.0,0.0,0.0)));
	slime->unserializeMobBase(slimeData->mob());
	return slime;
}
