#include "particle_renderer.hpp"

#include <iostream>

using namespace PixCraft;

bool Particle::operator<(const Particle other) const {
	return deathTime < other.deathTime;
}

void ParticleRenderer::init() {
	program.init(ShaderSources::particleVS, ShaderSources::particleFS);
	buffer.init(offsetof(Particle, x), offsetof(Particle, r),
		offsetof(Particle, size), sizeof(Particle));
	buffer.loadData(nullptr, MAX_PARTICLES, GL_STREAM_DRAW);
	checkGlErrors("particle renderer initialization");
	
	
	particles.insert(Particle {
		0, 39.55, 0,
		1, 0, 0, 1,
		0.1,
		0, 0, 0,
		1000
	});
}

void ParticleRenderer::render(glm::mat4 proj, glm::mat4 view, float fovy, int height) {
	auto particlesVector = particles.data();
	auto particleCount = particlesVector.size();
	buffer.updateData(particlesVector.data(), particleCount);
	
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	program.use();
	program.setUniform("view", view);
	program.setUniform("proj", proj);
	program.setUniform("fovY", (float) fovy);
	program.setUniform("winH", (float) height);
	buffer.bind();
	glDrawArrays(GL_POINTS, 0, particleCount);
	buffer.unbind();
	program.unuse();
	
	glDisable(GL_PROGRAM_POINT_SIZE);
}
