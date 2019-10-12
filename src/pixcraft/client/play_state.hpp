#pragma once

#include <vector>

#include "client.hpp"
#include "pixcraft/util/glm.hpp"

#include "face_renderer.hpp"
#include "chunk_renderer.hpp"
#include "entity_renderer.hpp"
#include "particle_renderer.hpp"
#include "hotbar.hpp"
#include "gui.hpp"
#include "console.hpp"

#include "pixcraft/server/world_module.hpp"
#include "pixcraft/server/world.hpp"
#include "pixcraft/server/chunk.hpp"
#include "pixcraft/server/mob.hpp"

namespace PixCraft {
	class PlayState : public GameState {
	public:
		PlayState(GameClient& client);
		
		void update(float dt) override;
		void render(int winWidth, int winHeight) override;
		
	private:
		static constexpr float SKY_COLOR[3] = {0.75f, 0.9f, 1.0f};
		static const int LOADS_PER_FRAME = 1;
		static constexpr float PLAYER_REACH = 5.0f;
		
		bool antialiasing;
		bool showDebug;
		bool paused;
		int renderDist;
		float fogStart, fogEnd;
		
		Console console;
		
		World world;
		Player* player;
		
		FaceRenderer faceRenderer;
		ChunkRenderer chunkRenderer;
		EntityRenderer entityRenderer;
		ParticleRenderer particleRenderer;
		Hotbar hotbar;
		
		ShaderProgram cursorProgram;
		IndexBuffer<glm::vec2> cursorBuffer;
		ShaderProgram colorOverlayProgram;
		VertexBuffer<glm::vec2> colorOverlayBuffer;
		ShaderProgram blockOverlayProgram;
		IndexBuffer<glm::vec3> blockOverlayBuffer;
		
		std::vector<Button> menuButtons;
		
		void setAntialiasing(bool enabled);
		void setRenderDistance(int renderDist);
	};
}
