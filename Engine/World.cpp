#include <Engine/World.h>

World* CreateWorld(MemoryBlock* parentBlock, Vector3 chunkDimMeteres)
{
    World* world = PushStruct((*parentBlock), World);

    uint64_t worldMemorySize = 512 * 1024 * 1024;
    uint8_t* worldBlock = (uint8_t*)PushSize(*parentBlock, worldMemorySize);

    world->m_Storage.Initialize(worldBlock, worldMemorySize);
    world->m_FirstFreeChunk = nullptr;
    world->m_FirstFreeBlock = nullptr;
    world->m_ChunkDimMeteres = chunkDimMeteres;

    return world;
}

WorldChunk** GetPointerToChunk(World* world, Vector3i at)
{
    //#TODO limit indexes as they can overflow when hashing
    int32_t hash = 17;
    hash = hash * 31 + at.x;
    hash = hash * 31 + at.y;
    hash = hash * 31 + at.z;

    hash = hash & 0x7FFFFFFF;

    uint32_t hashSlot = hash % (WORLD_CHUNK_HASH_SLOTS);
    WorldChunk** chunk = &world->m_ChunksHash[hashSlot];

    while (*chunk != nullptr && (*chunk)->m_Index != at)
    {
        chunk = &(*chunk)->m_NextInHash;
    }

    return chunk;
}

WorldChunk* GetChunk(World* world, int32_t x, int32_t y, int32_t z)
{
    WorldChunk* result = *GetPointerToChunk(world, Vector3i{ x, y, z });
    return result;
}

void PackEntity(World* world, Entity* entity, WorldPosition pos)
{
    WorldChunk** chunkPtr = GetPointerToChunk(world, pos.m_Index);
    WorldChunk* chunk = *chunkPtr;

    if (chunk == nullptr)
    {
        if (world->m_FirstFreeChunk == nullptr)
        {
            world->m_FirstFreeChunk = PushStruct(world->m_Storage, WorldChunk);
            world->m_FirstFreeChunk->m_NextInHash = nullptr;
        }

        chunk = world->m_FirstFreeChunk;
        world->m_FirstFreeChunk = chunk->m_NextInHash;

        chunk->m_Index = pos.m_Index;
        chunk->m_Block = nullptr;

        *chunkPtr = chunk;
    }

    if (chunk->m_Block == nullptr || chunk->m_Block->m_EntitiesCount == NUM_ENTITIES_PER_BLOCK)
    {
        if (world->m_FirstFreeBlock == nullptr)
        {
            world->m_FirstFreeBlock = PushStruct(world->m_Storage, WorldEntityBlock);
            world->m_FirstFreeBlock->m_EntitiesCount = 0;
        }

        WorldEntityBlock* newBlock = world->m_FirstFreeBlock;
        world->m_FirstFreeBlock = newBlock->m_Next;

        newBlock->m_Next = chunk->m_Block;

        chunk->m_Block = newBlock;
    }

    WorldEntityBlock* block = chunk->m_Block;

    block->m_Entities[block->m_EntitiesCount++] = *entity;
}
