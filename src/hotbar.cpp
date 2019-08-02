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
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 70, 0));
	faceRenderer.render(buffer, model);
}
