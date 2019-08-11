#include "blocks.hpp"

#include <vector>
#include <memory>

#include <iostream>

#include "textures.hpp"
#include "util.hpp"

#include "world.hpp"


namespace BlockRegistry {
	namespace {
		std::vector<std::unique_ptr<Block>> protoBlocks;
	}
	
	BlockId registerBlock(Block* block) {
		BlockId id = protoBlocks.size() + 1;
		block->setId(id);
		protoBlocks.emplace_back(block);
		return id;
	}
	
	const BlockId STONE_ID = registerBlock((new Block())->mainTexture(TEX(STONE)));
	const BlockId DIRT_ID = registerBlock((new Block())->mainTexture(TEX(DIRT)));
	const BlockId GRASS_ID = registerBlock(new GrassBlock());
	const BlockId TRUNK_ID = registerBlock(new TrunkBlock());
	const BlockId LEAVES_ID = registerBlock((new Block())->mainTexture(TEX(LEAVES))->rendering(BlockRendering::transparentCube));
	const BlockId WATER_ID = registerBlock(new WaterBlock());

	Block& fromId(BlockId id) {
		return *protoBlocks[id - 1];
	}
	
	unsigned int registeredCount() {
		return protoBlocks.size();
	}
}


Block::Block() :
	_id((BlockId) -1), _rendering(BlockRendering::opaqueCube), _mainTexture(TEX(PLACEHOLDER)), _collision(BlockCollision::solidCube) { }

uint8_t Block::getFaceTexture(uint8_t face) {
	return _mainTexture;
}

bool Block::update(World& world, int32_t x, int32_t y, int32_t z) { return false; }

Block* Block::rendering(BlockRendering rendering) { _rendering = rendering; return this; }
Block* Block::mainTexture(uint8_t texture) { _mainTexture = texture; return this; }
Block* Block::collision(BlockCollision collision) { _collision = collision; return this; }

BlockId Block::id() { return _id; }
BlockRendering Block::rendering() { return _rendering; }
uint8_t Block::mainTexture() { return _mainTexture; }
BlockCollision Block::collision() { return _collision; }

Block& Block::fromId(BlockId id) {
	return BlockRegistry::fromId(id);
}

void Block::setId(BlockId id) { _id = id; }


GrassBlock::GrassBlock() {
	mainTexture(TEX(GRASS_SIDE));
}

uint8_t GrassBlock::getFaceTexture(uint8_t face) {
	if(face == 4) {
		return TEX(DIRT);
	} else if(face == 5) {
		return TEX(GRASS_TOP);
	} else {
		return TEX(GRASS_SIDE);
	}
}

TrunkBlock::TrunkBlock() {
	mainTexture(TEX(TRUNK_SIDE));
}

uint8_t TrunkBlock::getFaceTexture(uint8_t face) {
	return face >= 4 ? 6 : 5;
}

WaterBlock::WaterBlock() {
	mainTexture(TEX(WATER));
	rendering(BlockRendering::translucentCube);
	collision(BlockCollision::fluidCube);
}

bool WaterBlock::update(World& world, int32_t x, int32_t y, int32_t z) {
	Block& water = Block::fromId(BlockRegistry::WATER_ID);
	bool hasGround = world.hasSolidBlock(x, y-1, z);
	if(hasGround) {
		for(int side = 0; side < 4; side++) {
			int32_t x2 = x + sideVectors[side][0];
			int32_t y2 = y + sideVectors[side][1];
			int32_t z2 = z + sideVectors[side][2];
			if(!world.hasBlock(x2, y2, z2)) {
				world.setBlock(x2, y2, z2, water);
			}
		}
	} else if(!world.hasBlock(x, y-1, z)) {
		world.setBlock(x, y-1, z, water);
	}
	return false;
}
