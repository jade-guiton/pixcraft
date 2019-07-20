#pragma once

#include <vector>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>

typedef uint32_t BlockId;

class Block {
public:
	virtual ~Block() = 0;
	
	virtual bool isOpaqueCube();
	virtual uint8_t getFaceTexture(uint8_t face);
	
	BlockId getId();
	static Block* fromId(BlockId id);
};

namespace BlockTypes {
	void registerTypes();
	
	BlockId getId(Block& block);
	Block* fromId(BlockId id);
};

class StoneBlock : public Block {
	uint8_t getFaceTexture(uint8_t face) override;
};

class DirtBlock : public Block {
	uint8_t getFaceTexture(uint8_t face) override;
};

class GrassBlock : public Block {
	uint8_t getFaceTexture(uint8_t face) override;
};
