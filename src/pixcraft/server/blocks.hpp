#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "pixcraft/client/textures.hpp"

#include "world_module.hpp"

namespace PixCraft {
	class Block;
	namespace BlockRegistry {
		BlockId registerBlock(Block* block);
		void defineBlocks();
		
		Block& fromId(BlockId id);
		unsigned int registeredCount();
		
		extern const BlockId STONE_ID;
		extern const BlockId DIRT_ID;
		extern const BlockId GRASS_ID;
		extern const BlockId TRUNK_ID;
		extern const BlockId LEAVES_ID;
		extern const BlockId WATER_ID;
		extern const BlockId PLANKS_ID;
	};
	
	enum class BlockRendering {
		opaqueCube, transparentCube, translucentCube
	};
	
	enum class BlockCollision {
		solidCube, fluidCube, air
	};
	
	class Block {
		friend BlockId BlockRegistry::registerBlock(Block* block);
		
	public:
		Block();
		virtual ~Block() = default;
		
		virtual void define();
		virtual TexId getFaceTexture(uint8_t face);
		virtual bool update(World& world, int32_t x, int32_t y, int32_t z);
		
		Block& rendering(BlockRendering rendering);
		Block& mainTexture(TexId texture);
		Block& collision(BlockCollision collision);
		
		BlockId id();
		BlockRendering rendering();
		TexId mainTexture();
		BlockCollision collision();
		
		static Block& fromId(BlockId id);

	private:
		BlockId _id;
		BlockRendering _rendering;
		TexId _mainTexture;
		BlockCollision _collision;
		
		void setId(BlockId id);
	};
	
	class GrassBlock : public Block {
	public:
		void define() override;
		TexId getFaceTexture(uint8_t face) override;
	};
	
	class TrunkBlock : public Block {
	public:
		void define() override;
		TexId getFaceTexture(uint8_t face) override;
	};
	
	class WaterBlock : public Block {
	public:
		void define() override;
		bool update(World& world, int32_t x, int32_t y, int32_t z) override;
	};
}
