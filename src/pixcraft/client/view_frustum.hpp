#pragma once

#include <cstdint>

#include "pixcraft/util/glm.hpp"

// http://www.cse.chalmers.se/~uffe/vfc.pdf (section about AABBs)

namespace PixCraft {
	struct ViewPlane {
		glm::vec4 plane;
		glm::vec3 nPoint;
	};
	
	struct ViewFrustum {
		ViewPlane left;
		ViewPlane right;
		ViewPlane bottom;
		ViewPlane top;
		ViewPlane far;
		ViewPlane near;
	};
	
	ViewFrustum computeViewFrustum(float fovy, float screenRatio, float near, float far, glm::vec3 pos, glm::vec3 orient);
	
	bool isVisible(ViewFrustum& vf, int32_t chunkX, int32_t chunkZ);
}
