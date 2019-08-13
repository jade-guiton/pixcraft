#include "slime.hpp"

#include <cmath>

#include "../util.hpp"

const float HEIGHT = 0.6;
const float RADIUS = HEIGHT / sqrt(TAU / 2);

Slime::Slime(World& world, glm::vec3 pos)
	: Mob(world, HEIGHT, RADIUS, false, true, pos, glm::vec3(0.0)) {}
