#pragma once

#include <Engine/Memory.h>
#include <Engine/Entity.h>
#include <Engine/WorldPosition.h>
#include <Math/Vector3.hpp>
#include <Math/Vector3i.hpp>

#define WORLD_CHUNK_HASH_SLOTS 9973
#define NUM_ENTITIES_PER_BLOCK 10

struct WorldEntityBlock
{
	Entity m_Entities[NUM_ENTITIES_PER_BLOCK];
	uint32_t m_EntitiesCount;

	WorldEntityBlock* m_Next;
};

struct WorldChunk
{
	Vector3i m_Index;

	WorldEntityBlock* m_Block;

	WorldChunk* m_NextInHash;
};

struct World
{
	MemoryBlock m_Storage;

	Vector3 m_ChunkDimMeteres;

	WorldChunk* m_ChunksHash[WORLD_CHUNK_HASH_SLOTS]; //random hash count

	WorldChunk* m_FirstFreeChunk;

	WorldEntityBlock* m_FirstFreeBlock;
};

World* CreateWorld(MemoryBlock* parentBlock, Vector3 chunkDimMeteres);
WorldChunk* GetChunk(World* world, int32_t x, int32_t y, int32_t z);
void PackEntity(World* world, Entity* entity, WorldPosition pos);
