#include "player.hpp"

#include <algorithm>

#include "util.hpp"

#include "blocks.hpp"
#include "world.hpp"

const char* movementModeNames[3] = {
	"Normal", "Flying", "Noclip"
};

Player::Player(World& world, glm::vec3 pos)
	: movementMode(MovementMode::normal), world(&world), _pos(pos), _orient(0.0), _speed(0.0), onGround(false) { }

glm::vec3 Player::pos() { return _pos; }
glm::vec3 Player::getFeetPos() { return _pos + glm::vec3(0, -EYE_HEIGHT, 0); }
void Player::pos(glm::vec3 pos) { _pos = pos; }
glm::vec3 Player::speed() { return _speed; }

glm::vec3 Player::orient() { return _orient; }
void Player::orient(glm::vec3 orient) {
	_orient = orient;
	if(_orient.x < -TAU/4) {
		_orient.x = -TAU/4;
	} else if(_orient.x > TAU/4) {
		_orient.x = TAU/4;
	}
}
void Player::rotate(glm::vec3 dorient) { orient(_orient + dorient); }

glm::vec3 Player::dirVector() {
	return glm::vec3(localToGlobalRot(orient()) * glm::vec4(0.0, 0.0, -1.0, 1.0));
}

std::tuple<bool, int,int,int> Player::castRay(float maxDist, bool offset, bool hitFluids) {
	return world->raycast(pos(), dirVector(), maxDist, offset, hitFluids);
}

bool Player::canFly() { return movementMode == MovementMode::flying || movementMode == MovementMode::noClip; }
bool Player::collidesWithBlocks() { return movementMode == MovementMode::normal || movementMode == MovementMode::flying; }
float Player::getMaxHorSpeed() {
	if(movementMode == MovementMode::normal) {
		return WALK_SPEED;
	} else if(movementMode == MovementMode::flying) {
		return FLY_SPEED;
	} else {
		return NOCLIP_SPEED;
	}
}

std::pair<glm::vec3, glm::vec3> Player::getBoundingBox() {
	glm::vec3 feetPos = getFeetPos();
	glm::vec3 hor = glm::vec3(RADIUS, 0, RADIUS);
	glm::vec3 ver = glm::vec3(0, HEIGHT, 0);
	return std::pair<glm::vec3, glm::vec3>(feetPos - hor, feetPos + hor + ver);
}

int Player::getUnderwaterLevel() {
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
				Block* block = world->getBlock(x, y, z);
				if(block != nullptr && block->collision() == BlockCollision::fluidCube) {
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
	float relWaterLevel = waterLevel + 0.5 - c1.y;
	int level = 1 + (relWaterLevel >= 1.1);
	return level;
}

void Player::move(std::tuple<int,int,bool,bool> mvtKeys, float dt) {
	glm::mat4 yRot = glm::mat4(1.0f);
	yRot = glm::rotate(yRot, _orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	
	int dx, dz; bool up, down;
	std::tie(dx, dz, up, down) = mvtKeys;
	float mvtSpeed = getMaxHorSpeed();
	
	if(canFly()) {
		int dy = up - down;
		if(dx != 0 || dy != 0 || dz != 0) {
			_speed = glm::normalize(glm::vec3(dx, dy, dz));
			_speed = mvtSpeed * glm::vec3(yRot * glm::vec4(_speed, 1.0f));
		} else {
			_speed = glm::vec3(0);
		}
	} else {
		glm::vec3 horSpeed(0);
		if(dx != 0 || dz != 0) {
			horSpeed = glm::normalize(glm::vec3(dx, 0, dz));
			horSpeed = mvtSpeed * glm::vec3(yRot * glm::vec4(horSpeed, 1.0f));
		} else {
			horSpeed = glm::vec3(0);
		}
		_speed.x = horSpeed.x; _speed.z = horSpeed.z;
		int underwaterLevel = getUnderwaterLevel();
		if(up) {
			if(onGround && underwaterLevel < 2) {
				_speed.y = JUMP_SPEED;
			}
			if(underwaterLevel == 2 && _speed.y <= SWIM_SPEED) { // above waist
				_speed.y += dt*SWIM_ACCEL;
				if(_speed.y >= SWIM_SPEED) _speed.y = SWIM_SPEED;
			}
		}
		if(underwaterLevel > 0) {
			_speed.y -= dt*(GRAVITY - BUOYANCY);
		} else {
			_speed.y -= dt*GRAVITY;
		}
	}
	onGround = false;
	
	glm::vec3 dpos = dt * _speed;
	if(collidesWithBlocks()) {
		float verBarrier = std::max(std::min(std::abs(_speed.y)/30, 0.5f), 0.05f);
		float margin = 0.001;
		
		if(dpos.y < 0) {
			glm::vec3 feetPos = getFeetPos() + glm::vec3(0, dpos.y, 0);
			float verDispl = world->collideDiskVer(feetPos, RADIUS, verBarrier, margin);
			if(verDispl > 0) {
				dpos.y += verDispl;
				_speed.y = 0.0;
				onGround = true;
			}
		} else if(dpos.y > 0) {
			glm::vec3 headPos = getFeetPos() + glm::vec3(0, dpos.y + HEIGHT, 0);
			float verDispl = world->collideDiskVer(headPos, RADIUS, verBarrier, margin);
			if(verDispl < 0) {
				dpos.y += verDispl;
				_speed.y = 0.0;
			}
		}
		
		glm::vec3 feetPos = getFeetPos() + glm::vec3(dpos.x, 0, dpos.z);
		glm::vec2 horDispl = world->collideCylHor(feetPos, RADIUS, HEIGHT, margin);
		dpos.x += horDispl.x;
		dpos.z += horDispl.y;
	}
	
	_pos += dpos;
}
