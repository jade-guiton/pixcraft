#include "particle_renderer.hpp"

#include <iostream>
#include <cmath>
#include <random>

#include "../util/util.hpp"

using namespace PixCraft;

constexpr float GRAVITY = 16.0f;

bool Particle::operator<(const Particle other) const {
	return deathTime < other.deathTime;
}

void ParticleRenderer::init() {
	elapsedFrames = 0;
	
	program.init(ShaderSources::particleVS, ShaderSources::particleFS);
	buffer.init(offsetof(Particle, x), offsetof(Particle, size),
		offsetof(Particle, blockTex), offsetof(Particle, tx), sizeof(Particle));
	buffer.loadData(nullptr, MAX_PARTICLES, GL_STREAM_DRAW);
	checkGlErrors("particle renderer initialization");
	
	std::random_device randDev;
	random.seed(randDev());
}

void ParticleRenderer::spawnBlockBits(glm::vec3 blockPos, TexId blockTex) {
	int n = 1;
	for(int dx = -n; dx <= n; ++dx) {
		for(int dy = -n; dy <= n; ++dy) {
			for(int dz = -n; dz <= n; ++dz) {
				int tx = ((dx+n) + (dz+n)) % TextureManager::BLOCK_TEX_SIZE;
				int ty = (dy+n) % TextureManager::BLOCK_TEX_SIZE;
				std::uniform_real_distribution<float> realDistr(-1, 1);
				float x = blockPos.x + (float) dx/(2*n+1) + realDistr(random)/12;
				float y = blockPos.y + (float) dy/(2*n+1) + realDistr(random)/12;
				float z = blockPos.z + (float) dz/(2*n+1) + realDistr(random)/12;
				particles.insert(Particle {
					x, y, z,
					(float) 0.2,
					blockTex,
					(float) tx / TextureManager::BLOCK_TEX_SIZE, (float) ty / TextureManager::BLOCK_TEX_SIZE,
					0.3f*dx/n + 0.3f*realDistr(random), 1.5f + 0.5f*realDistr(random), 0.3f*dz/n + 0.3f*realDistr(random),
					elapsedFrames + 40 + (int) (realDistr(random)*10)
				});
			}
		}
	}
}

void ParticleRenderer::update(float dt) {
	float t = elapsedFrames / 60.0;
	glm::vec3 color = hslToRgb(glm::vec3(std::fmod(t, 1.0), 1.0, 0.5));
	float phi = t * 6.2832;
	if(elapsedFrames % 2 == 0) {
		particles.insert(Particle {
			(float) (0 + 0.5*cos(phi)), (float) (39 + 0.5*sin(phi)), (float) (0 + 0.5*sin(phi/2)),
			(float) 0.1,
			TextureManager::LEAVES,
			(float) (0.5 + 0.4*cos(phi)), (float) (0.5 + 0.4*sin(phi)),
			0, 0, 0,
			elapsedFrames + 110
		});
	}
	
	while(!particles.empty() && (particles.size() > MAX_PARTICLES || particles.min().deathTime <= elapsedFrames)) {
		particles.removeMin();
	}
	
	for(auto it = particles.iter(); !it.done(); ++it) {
		it->vy -= GRAVITY * dt;
		it->x += it->vx * dt;
		it->y += it->vy * dt;
		it->z += it->vz * dt;
	}
	
	elapsedFrames++;
}

void ParticleRenderer::render(glm::mat4 proj, glm::mat4 view, float fovy, int height) {
	std::vector<Particle> particlesVector;
	for(auto it = particles.iter(); !it.done(); ++it) {
		particlesVector.push_back(*it);
	}
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
	
	program.setUniform("texArray", (uint32_t) 0);
	TextureManager::bindBlockTextureArray();
	buffer.bind();
	glDrawArrays(GL_POINTS, 0, particleCount);
	buffer.unbind();
	program.unuse();
	
	glDisable(GL_PROGRAM_POINT_SIZE);
}
