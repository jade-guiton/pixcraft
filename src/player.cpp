#include "player.hpp"

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

std::tuple<bool, int,int,int> Player::castRay(float maxDist, bool offset) {
	return world->raycast(pos(), dirVector(), maxDist, offset);
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
		if(up && onGround) {
			_speed.y = JUMP_SPEED;
		}
		_speed.y -= dt*GRAVITY;
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
