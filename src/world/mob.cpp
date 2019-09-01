#include "mob.hpp"

#include <algorithm>

#include "../util.hpp"

#include "blocks.hpp"
#include "world.hpp"

using namespace PixCraft;

const float BUOYANCY = 20.0f;


glm::vec3 Mob::pos() { return _pos; }
void Mob::pos(glm::vec3 pos) { _pos = pos; }
glm::vec3 Mob::speed() { return _speed; }

glm::vec3 Mob::orient() { return _orient; }
void Mob::orient(glm::vec3 orient) { _orient = orient; }
void Mob::rotate(glm::vec3 dorient) {
	_orient += dorient;
	if(_orient.x > TAU/4) _orient.x = TAU/4;
	if(_orient.x < -TAU/4) _orient.x = -TAU/4;
}

glm::vec3 Mob::dirVector() {
	return glm::vec3(localToGlobalRot(orient()) * glm::vec4(0.0, 0.0, -1.0, 1.0));
}

std::pair<glm::vec3, glm::vec3> Mob::getBoundingBox() {
	glm::vec3 hor = glm::vec3(radius, 0, radius);
	glm::vec3 ver = glm::vec3(0, height, 0);
	return std::pair<glm::vec3, glm::vec3>(_pos - hor, _pos + hor + ver);
}

bool Mob::isInsideBlock(int32_t x, int32_t y, int32_t z) {
	return cylinderBlockCollision(_pos, radius, height, x, y, z);
}

float Mob::getWaterHeight() {
	glm::vec3 c1, c2;
	std::tie(c1, c2) = getBoundingBox();
	int minX, minY, minZ;
	std::tie(minX, minY, minZ) = getBlockCoordsAt(c1);
	int maxX, maxY, maxZ;
	std::tie(maxX, maxY, maxZ) = getBlockCoordsAt(c2);
	int waterLevel = 0;
	for(int32_t y = minY; y <= maxY; ++y) {
		for(int32_t x = minX; x <= maxX; ++x) {
			for(int32_t z = minZ; z <= maxZ; ++z) {
				Block* block = world.getBlock(x, y, z);
				if(block == &Block::fromId(BlockRegistry::WATER_ID)) {
					waterLevel = y;
					break;
				}
			}
			if(waterLevel == y) {
				break;
			}
		}
	}
	if(waterLevel == 0) return 0;
	return waterLevel + 0.5 - _pos.y;
}

void Mob::update(float dt) {
	if(!canFly) {
		if(getWaterHeight() > 0) {
			_speed.y -= dt*(GRAVITY - BUOYANCY);
		} else {
			_speed.y -= dt*GRAVITY;
		}
	}
	
	onGround = false;
	glm::vec3 dpos = dt * _speed;
	if(collidesWithBlocks) {
		float verBarrier = std::max(std::min(std::abs(_speed.y)/30, 0.5f), 0.05f);
		float margin = 0.001;
		
		if(dpos.y < 0) {
			glm::vec3 feetPos = _pos + glm::vec3(0, dpos.y, 0);
			float verDispl = world.collideDiskVer(feetPos, radius, verBarrier, margin);
			if(verDispl > 0) {
				dpos.y += verDispl;
				_speed.y = 0.0;
				onGround = true;
			}
		} else if(dpos.y > 0) {
			glm::vec3 headPos = _pos + glm::vec3(0, dpos.y + height, 0);
			float verDispl = world.collideDiskVer(headPos, radius, verBarrier, margin);
			if(verDispl < 0) {
				dpos.y += verDispl;
				_speed.y = 0.0;
			}
		}
		
		glm::vec3 feetPos = _pos + glm::vec3(dpos.x, 0, dpos.z);
		glm::vec2 horDispl = world.collideCylHor(feetPos, radius, height, margin);
		dpos.x += horDispl.x;
		dpos.z += horDispl.y;
	}
	
	_pos += dpos;
}

Mob::Mob(World& world, float height, float radius, bool canFly, bool collidesWithBlocks, glm::vec3 pos, glm::vec3 orient)
	: world(world), height(height), radius(radius), canFly(canFly), collidesWithBlocks(collidesWithBlocks), onGround(false),
	  _pos(pos), _orient(orient), _speed(0.0) {}