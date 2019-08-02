#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <memory>

typedef uint32_t BlockId;

class Block;

namespace BlockRegistry {
	BlockId nextId();
	void registerBlocks();
	
	Block& fromId(BlockId id);
	int registeredIds();
};

class Block {
	friend void BlockRegistry::registerBlocks();
	
public:
	virtual ~Block();
	
	virtual uint8_t getFaceTexture(uint8_t face);
	
	BlockId id();
	bool isOpaqueCube();
	uint8_t mainTexture();
	
	static Block& fromId(BlockId id);
	
protected:
	Block();
	
	Block& isOpaqueCube(bool isOpaqueCube);
	Block& mainTexture(uint8_t texture);

private:
	BlockId _id;
	bool _isOpaqueCube;
	int8_t _mainTexture;
};

class GrassBlock : public Block {
	uint8_t getFaceTexture(uint8_t face) override;
};
