#pragma once

#include <cstddef>

#include "glfw.hpp"
#include "pixcraft/util/glm.hpp"

#include "shaders.hpp"
#include "textures.hpp"

#include "pixcraft/server/world_module.hpp"

namespace PixCraft {
	class EntityModel {
	public:
		template<size_t vertexCount, size_t indexCount>
		void init(TexId texture, const std::array<float, vertexCount>& vertices,
			const std::array<unsigned int, indexCount>& indices, glm::mat4 preModel);
		
		glm::mat4 preModel();
		
		void bindTexture();
		void render();
		
	private:
		IndexBuffer<glm::vec3, glm::vec2> buffer;
		TexId texture;
		glm::mat4 _preModel;
	};
	
	class EntityRenderer {
	public:
		EntityRenderer();
		void init();
		
		void renderEntities(World& world, glm::mat4 proj, glm::mat4 view, RenderParams params);
		
	private:
		ShaderProgram program;
		
		EntityModel slimeModel;
		
		void startRendering(glm::mat4 proj, glm::mat4 view, RenderParams params);
		void render(EntityModel& model, glm::mat4 modelMat);
		void stopRendering();
	};
}