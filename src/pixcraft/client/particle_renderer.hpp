#pragma once

#include <vector>

#include "glfw.hpp"
#include "shaders.hpp"

#include "../util/pairing_heap.hpp"

namespace PixCraft {
	struct Particle {
		float x, y, z;
		float r, g, b, a;
		float size;
		float vx, vy, vz;
		float deathTime;
		
		bool operator<(const Particle other) const;
	};
	
	class ParticleRenderer {
	public:
		void init();
		
		void render(glm::mat4 proj, glm::mat4 view, float fovy, int height);
		
	private:
		const unsigned int MAX_PARTICLES = 128;
		
		ShaderProgram program;
		VertexBuffer<glm::vec3, glm::vec4, float> buffer;
		
		PairingHeap<Particle> particles;
	};
}
