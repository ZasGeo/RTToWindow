#include <Engine/Simulation.h>

void UpdateSimulation(Simulation* sim, World* world, WorldPosition newOrigin)
{
    sim->m_Origin = newOrigin;

    Vector3 chunkDim = world->m_ChunkDimMeteres;
    Vector3 simCornerOffset = sim->m_SimulationDimMetres * 0.5f;

    WorldPosition minCorner = sim->m_Origin;
    minCorner.m_Offset -= simCornerOffset;
    minCorner = FitIntoChunk(minCorner, chunkDim);

    WorldPosition maxCorner = sim->m_Origin;
    maxCorner.m_Offset += simCornerOffset;
    maxCorner = FitIntoChunk(maxCorner, chunkDim);

    for (uint32_t entIdx = sim->m_Entities.m_NumEntities; entIdx > 0; --entIdx)
    {
        Entity* ent = sim->m_Entities.m_Entities + entIdx - 1;

        Vector3i chunkIdx = ent->m_WorldPos.m_Index;

        bool isInChunk = 
            chunkIdx.x >= minCorner.m_Index.x &&
            chunkIdx.y >= minCorner.m_Index.y &&
            chunkIdx.z >= minCorner.m_Index.z &&
            chunkIdx.x <= maxCorner.m_Index.x &&
            chunkIdx.y <= maxCorner.m_Index.y &&
            chunkIdx.z <= maxCorner.m_Index.z;

        if (!isInChunk)
        {
            PackEntity(world, ent, ent->m_WorldPos);

            RemoveEntity(&sim->m_Entities, ent->m_Id);
        }

    }

    for (int32_t z = minCorner.m_Index.z; z <= maxCorner.m_Index.z; ++z)
    {
        for (int32_t y = minCorner.m_Index.y; y <= maxCorner.m_Index.y; ++y)
        {
            for (int32_t x = minCorner.m_Index.x; x <= maxCorner.m_Index.x; ++x)
            {
                WorldChunk* chunk = GetChunk(world, x, y, z);
                if (chunk != nullptr)
                {
                    WorldEntityBlock* block = chunk->m_Block;
                    while (block != nullptr)
                    {
                        for (uint32_t entityIndex = 0; entityIndex < block->m_EntitiesCount; ++entityIndex)
                        {
                            //TODO store actually packed data
                            Entity* packedEntity = block->m_Entities + entityIndex;

                            Entity* simEntity = AddEntity(&sim->m_Entities);

                            //TODO unpack by component
                            EntityId generatedId = simEntity->m_Id;
                            *simEntity = *packedEntity;
                            simEntity->m_Id = generatedId;
                            simEntity->m_SimPos = Substruct(simEntity->m_WorldPos, sim->m_Origin, chunkDim);
                        }

                        block->m_EntitiesCount = 0;

                        block = block->m_Next;
                    }

                    //TODO recycle blocks and chunk
                }
            }
        }
    }
}
