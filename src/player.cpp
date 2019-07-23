#include "player.hpp"

Player::Player(World& world, glm::vec3 pos)
	: movementMode(MovementMode::normal), world(&world), _pos(pos), _orient(0.0), ySpeed(0.0), onGround(false) { }

glm::vec3 Player::pos() { return _pos; }
glm::vec3 Player::getFeetPos() { return _pos + glm::vec3(0, -EYE_HEIGHT, 0); }
void Player::pos(glm::vec3 pos) { _pos = pos; }
void Player::move(glm::vec3 dpos) { _pos += dpos; }

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

void Player::move(std::tuple<int,int,bool,bool> mvtKeys, float dt) {
	glm::mat4 yRot = glm::mat4(1.0f);
	yRot = glm::rotate(yRot, _orient.y, glm::vec3(0.0f, 1.0f, 0.0f));
	
	int dx, dz; bool up, down;
	std::tie(dx, dz, up, down) = mvtKeys;
	if(canFly()) {
		int dy = up - down;
		if(dx != 0 || dy != 0 || dz != 0) {
			glm::vec3 mvt = glm::normalize(glm::vec3(dx, dy, dz));
			move(dt * FLY_SPEED * glm::vec3(yRot * glm::vec4(mvt, 1.0f)));
		}
		ySpeed = 0.0;
	} else {
		if(dx != 0 || dz != 0) {
			glm::vec2 mvt = glm::normalize(glm::vec2(dx, dz));
			move(dt * WALK_SPEED * glm::vec3(yRot * glm::vec4(mvt.x, 0.0, mvt.y, 1.0f)));
		}
		if(up && onGround) {
			ySpeed = JUMP_SPEED;
		}
		ySpeed -= dt*GRAVITY;
		_pos.y += dt*ySpeed;
	}
	onGround = false;
}

void Player::collide() {
	if(!collidesWithBlocks()) return;
	
	float verBarrier = std::min(std::abs(ySpeed)/15, 0.5f);
	
	glm::vec3 feetPos = getFeetPos();
	float verDispl = world->collideDiskVer(feetPos, RADIUS, verBarrier);
	if(verDispl > 0) {
		_pos.y += verDispl;
		if(ySpeed < 0) {
			ySpeed = 0.0;
			onGround = true;
		}
	}
	
	glm::vec3 headPos = getFeetPos() + glm::vec3(0, HEIGHT, 0);
	verDispl = world->collideDiskVer(headPos, RADIUS, verBarrier);
	if(verDispl < 0) {
		_pos.y += verDispl;
		if(ySpeed > 0) {
			ySpeed = 0.0;
		}
	}
	
	feetPos = getFeetPos();
	glm::vec2 horDispl = world->collideCylHor(feetPos, RADIUS, HEIGHT);
	_pos.x += horDispl.x;
	_pos.z += horDispl.y;
}
