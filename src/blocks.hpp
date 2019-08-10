#pragma once

#include <cstdint>
#include <vector>
#include <memory>

typedef uint32_t BlockId;

class Block;
namespace BlockRegistry {
	BlockId registerBlock(Block* block);
	
	Block& fromId(BlockId id);
	unsigned int registeredCount();
	
	extern const BlockId STONE_ID;
	extern const BlockId DIRT_ID;
	extern const BlockId GRASS_ID;
	extern const BlockId TRUNK_ID;
	extern const BlockId LEAVES_ID;
	extern const BlockId WATER_ID;
};

class Block {
	friend BlockId BlockRegistry::registerBlock(Block* block);
	
public:
	Block();
	virtual ~Block() = default;
	
	virtual uint8_t getFaceTexture(uint8_t face);
	
	Block* isOpaqueCube(bool isOpaqueCube);
	Block* mainTexture(uint8_t texture);
	
	BlockId id();
	bool isOpaqueCube();
	uint8_t mainTexture();
	
	static Block& fromId(BlockId id);

private:
	BlockId _id;
	bool _isOpaqueCube;
	int8_t _mainTexture;
	
	void setId(BlockId id);
};

class GrassBlock : public Block {
	using Block::Block;
	uint8_t getFaceTexture(uint8_t face) override;
};

class TrunkBlock : public Block {
	using Block::Block;
	uint8_t getFaceTexture(uint8_t face) override;
};
