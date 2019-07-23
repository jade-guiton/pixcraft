#include "player.hpp"

Player::Player(glm::vec3 pos) : _pos(pos), _orient(0.0) { }

glm::vec3 Player::pos() { return _pos; }
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

Ray* Player::castRay() {
	return new Ray(pos(), orient());
}
