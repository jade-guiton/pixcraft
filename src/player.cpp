#include "player.hpp"

Player::Player(World& world, glm::vec3 pos) : world(&world), _pos(pos), _orient(0.0) { }

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

void Player::collideOut() {
	glm::vec3 feetPos = getFeetPos();
	glm::vec3 bodyPos = getFeetPos();
	bodyPos.y += HEIGHT / 2;
	glm::vec3 headPos = getFeetPos();
	headPos.y += HEIGHT;
	
	const float horBarrier = 0.0;
	const float verBarrier = 0.2;
	
	int feetHitX, feetHitY, feetHitZ;
	std::tie(feetHitX, feetHitY, feetHitZ) = world->collideHorDisk(feetPos, RADIUS, horBarrier, verBarrier);
	if(feetHitY == 1) {
		_pos = snapToEdge(feetPos, RADIUS, 0, 1, 0) + glm::vec3(0, EYE_HEIGHT, 0);
		collideOut();
		return;
	}
	
	int headHitX, headHitY, headHitZ;
	std::tie(headHitX, headHitY, headHitZ) = world->collideHorDisk(headPos, RADIUS, horBarrier, verBarrier);
	if(headHitY == -1) {
		_pos = snapToEdge(headPos, RADIUS, 0, 1, 0) + glm::vec3(0, EYE_HEIGHT - HEIGHT, 0);
		collideOut();
		return;
	}
	
	int bodyHitX, bodyHitY, bodyHitZ;
	std::tie(bodyHitX, bodyHitY, bodyHitZ) = world->collideHorDisk(bodyPos, RADIUS, horBarrier, verBarrier);
	
	int dispX = collateCollisions(feetHitX, bodyHitX, headHitX);
	int dispZ = collateCollisions(feetHitZ, bodyHitZ, headHitZ);
	if(dispX != 0 || dispZ != 0) {
		_pos = snapToEdge(_pos, RADIUS, dispX, 0, dispZ);
		return;
	}
	
	/*
	int bodyHitX, bodyHitY, bodyHitZ;
	std::tie(bodyHitX, bodyHitY, bodyHitZ) = world->collideHorDisk(bodyPos, RADIUS, 0.1);
	if(bodyHitX != 0 || bodyHitZ != 0) {
		_pos = snapToEdge(_pos, RADIUS, bodyHitX, 0, bodyHitZ);
		return;
	}
	*/
	
	/*
	int headHitX, headHitY, headHitZ;
	std::tie(headHitX, headHitY, headHitZ) = world->collideHorDisk(headPos, RADIUS, 0.1);
	if(headHitY == -1 || headHitX != 0 || headHitZ != 0) {
		glm::vec3 newPos;
		if(headHitY == -1) {
			newPos = snapToEdge(headPos, RADIUS, 0, headHitY, 0);
		} else {
			newPos = snapToEdge(headPos, RADIUS, headHitX, 0, headHitZ);
		}
		newPos.y += EYE_HEIGHT - HEIGHT;
		_pos = newPos;
		return;
	}
	*/
}
