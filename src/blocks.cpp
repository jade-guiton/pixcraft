#include "blocks.hpp"

Block::~Block() { }

uint8_t Block::getFaceTexture(uint8_t face) {
	return _mainTexture;
}

BlockId Block::id() { return _id; }
bool Block::isOpaqueCube() { return _isOpaqueCube; }
uint8_t Block::mainTexture() { return _mainTexture; }

Block& Block::fromId(BlockId id) {
	return BlockRegistry::fromId(id);
}


Block::Block() : _id(BlockRegistry::nextId()), _isOpaqueCube(true), _mainTexture(0) { }

Block& Block::isOpaqueCube(bool isOpaqueCube) { _isOpaqueCube = isOpaqueCube; return *this; }
Block& Block::mainTexture(uint8_t texture) { _mainTexture = texture; return *this; }


namespace BlockRegistry {
	namespace {
		std::vector<std::unique_ptr<Block>> protoBlocks;
		
		Block& registerBlock(Block* block) {
			protoBlocks.emplace_back(std::unique_ptr<Block>(block));
			return *block;
		}
	}
	
	BlockId nextId() {
		return protoBlocks.size() + 1;
	}
	
	void registerBlocks() {
		registerBlock(new Block()).mainTexture(1); // stone
		registerBlock(new Block()).mainTexture(2); // dirt
		registerBlock(new GrassBlock()); // grass
		registerBlock(new TrunkBlock()); // grass
	}

	Block& fromId(BlockId id) {
		return *protoBlocks[id - 1];
	}
}

uint8_t GrassBlock::getFaceTexture(uint8_t face) {
	if(face == 4) {
		return 2;
	} else if(face == 5) {
		return 4;
	} else {
		return 3;
	}
}

uint8_t TrunkBlock::getFaceTexture(uint8_t face) {
	return face >= 4 ? 6 : 5;
}
