#include "view_frustum.hpp"

#include <cmath>

#include "world/world_module.hpp"
#include "util.hpp"

using namespace PixCraft;

// Takes a normal and a point in camera space, and returns the corresponding plane in world space.
// Also computes the n- and p-vertices of AABBs relative to the plane.
ViewPlane computeViewPlane(glm::vec3 normal, glm::vec3 point, glm::mat4 trans) {
	glm::vec4 camPlane = glm::vec4(normal, glm::dot(-normal, point));
	ViewPlane viewPlane;
	viewPlane.plane = trans * camPlane;
	viewPlane.nPoint = glm::vec3(CHUNK_SIZE * (viewPlane.plane.x < 0), CHUNK_HEIGHT * (viewPlane.plane.y < 0),
		CHUNK_SIZE * (viewPlane.plane.z < 0));
	return viewPlane;
}

ViewFrustum PixCraft::computeViewFrustum(float fovy, float screenRatio, float near, float far, glm::vec3 pos, glm::vec3 orient) {
	float fovx = 2*atan(screenRatio * tan(fovy / 2));
	ViewFrustum vf;
	glm::vec3 forward(0.0f, 0.0f, -1.0f);
	glm::mat4 id(1.0f);
	// To transform a plane, use the transposed inverse of the point transformation:
	glm::mat4 trans = glm::transpose(globalToLocal(pos, orient));
	vf.left = computeViewPlane(glm::mat3(glm::rotate(id, TAU/4 + fovx/2, glm::vec3(0.0f, 1.0f, 0.0f))) * forward,
		glm::vec3(0.0f), trans);
	vf.right = computeViewPlane(glm::mat3(glm::rotate(id, TAU/4 + fovx/2, glm::vec3(0.0f, -1.0f, 0.0f))) * forward,
		glm::vec3(0.0f), trans);
	vf.bottom = computeViewPlane(glm::mat3(glm::rotate(id, TAU/4 + fovy/2, glm::vec3(-1.0f, 0.0f, 0.0f))) * forward,
		glm::vec3(0.0f), trans);
	vf.top = computeViewPlane(glm::mat3(glm::rotate(id, TAU/4 + fovy/2, glm::vec3(1.0f, 0.0f, 0.0f))) * forward,
		glm::vec3(0.0f), trans);
	vf.far = computeViewPlane(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -far), trans);
	vf.near = computeViewPlane(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -near), trans);
	return vf;
}

bool PixCraft::isVisible(ViewFrustum& vf, int32_t chunkX, int32_t chunkZ) {
	glm::vec3 corner(CHUNK_SIZE*chunkX - 0.5, -0.5, CHUNK_SIZE*chunkZ - 0.5);
	
	glm::vec3 nPoint;
	nPoint = corner + vf.left.nPoint;
	if(glm::dot(glm::vec4(nPoint, 1.0f), vf.left.plane) > 0) return false;
	nPoint = corner + vf.right.nPoint;
	if(glm::dot(glm::vec4(nPoint, 1.0f), vf.right.plane) > 0) return false;
	nPoint = corner + vf.bottom.nPoint;
	if(glm::dot(glm::vec4(nPoint, 1.0f), vf.bottom.plane) > 0) return false;
	nPoint = corner + vf.top.nPoint;
	if(glm::dot(glm::vec4(nPoint, 1.0f), vf.top.plane) > 0) return false;
	nPoint = corner + vf.far.nPoint;
	if(glm::dot(glm::vec4(nPoint, 1.0f), vf.far.plane) > 0) return false;
	nPoint = corner + vf.near.nPoint;
	if(glm::dot(glm::vec4(nPoint, 1.0f), vf.near.plane) > 0) return false;
	
	return true;
}