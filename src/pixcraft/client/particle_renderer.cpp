#include "particle_renderer.hpp"

#include <iostream>
#include <cmath>

#include "../util/util.hpp"

using namespace PixCraft;

bool Particle::operator<(const Particle other) const {
	return deathTime < other.deathTime;
}

void ParticleRenderer::init() {
	elapsedFrames = 0;
	
	program.init(ShaderSources::particleVS, ShaderSources::particleFS);
	buffer.init(offsetof(Particle, x), offsetof(Particle, r),
		offsetof(Particle, size), sizeof(Particle));
	buffer.loadData(nullptr, MAX_PARTICLES, GL_STREAM_DRAW);
	checkGlErrors("particle renderer initialization");
}

void ParticleRenderer::update(float dt) {
	float t = elapsedFrames / 60.0;
	glm::vec3 color = hslToRgb(glm::vec3(std::fmod(t, 1.0), 1.0, 0.5));
	float phi = t * 6.2832;
	particles.insert(Particle {
		0 + (float) 0.5*cos(phi), 39 + (float) 0.5*sin(phi), (float) 0.5*sin(phi/2),
		color.r, color.g, color.b, 1,
		0.1,
		0, 0, 0,
		elapsedFrames + 110
	});
	while(!particles.empty() && particles.min().deathTime <= elapsedFrames) {
		particles.removeMin();
	}
	elapsedFrames++;
}

void ParticleRenderer::render(glm::mat4 proj, glm::mat4 view, float fovy, int height) {
	auto particlesVector = particles.data();
	auto particleCount = particlesVector.size();
	if(particleCount > MAX_PARTICLES) {
		particleCount = MAX_PARTICLES;
		std::cout << "Too many particles!" << std::endl;
	}
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
