#include "blocks.hpp"

Block::~Block() { }

bool Block::isOpaqueCube() {
	return true;
}

uint8_t Block::getFaceTexture(uint8_t face) {
	return 0; // placeholder texture
}

BlockId Block::getId() {
	return BlockTypes::getId(*this);
}

Block* Block::fromId(BlockId id) {
	return BlockTypes::fromId(id);
}

namespace BlockTypes {
	namespace {
		std::vector<Block* (*)(void)> creators;
		std::unordered_map<std::type_index, BlockId> ids;
		
		template<class T>
		Block* createBlock() {
			return new T;
		}
		
		template<class T>
		void registerType() {
			creators.push_back(&createBlock<T>);
			BlockId id = (BlockId) creators.size();
			ids[std::type_index(typeid(T))] = id;
		}
	}
	
	void registerTypes() {
		registerType<StoneBlock>();
		registerType<DirtBlock>();
		registerType<GrassBlock>();
	}
	
	BlockId getId(Block& block) {
		try {
			return ids.at(std::type_index(typeid(block)));
		} catch(std::out_of_range& err) {
			throw std::runtime_error("Unregistered block type!");
		}
	}

	Block* fromId(BlockId id) {
		return creators[id - 1]();
	}
}

uint8_t StoneBlock::getFaceTexture(uint8_t face) {
	return 1;
}

uint8_t DirtBlock::getFaceTexture(uint8_t face) {
	return 2;
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
