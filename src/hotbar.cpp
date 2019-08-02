#include "hotbar.hpp"

Hotbar::Hotbar(FaceRenderer& renderer)
	: faceRenderer(renderer) { }

void Hotbar::init() {
	buffer.init(faceRenderer, 6);
	setBlock(1);
}

void Hotbar::setBlock(BlockId blockId) {
	Block& block = Block::fromId(blockId);
	
	std::vector<FaceData> faces;
	for(uint8_t side = 0; side < 6; ++side) {
		faces.push_back(FaceData {
			0, 0, 0, side, block.getFaceTexture(side)
		});
	}
	buffer.load(faces);
}

void Hotbar::render() {
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), 0.1f, glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.35, -0.25, -0.25));
	model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
	faceRenderer.render(buffer, model);
}
