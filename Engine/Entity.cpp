#include <cassert>

#include <Engine/Entity.h>

uint32_t EntityIdHash(EntityId id)
{
    //TODO BETTER HASH FUNCTION
    uint32_t result = (id.m_Value - 1) % MAX_ENTITIES_NUM;
    return result;
}
EntityLookUpEntry* GetEntityLookUpEntry(EntityStorage* storage, EntityId id)
{
    EntityLookUpEntry* result = nullptr;
    uint32_t currentIndex = EntityIdHash(id);
    for (uint32_t i = 0; i < MAX_ENTITIES_NUM; ++i)
    {
        EntityLookUpEntry* entry = storage->m_EntityHash + currentIndex;
        if (entry->m_Id == id)
        {
            result = entry;
            break;
        }
        else
        {
            currentIndex = (currentIndex + 1) % MAX_ENTITIES_NUM;
        }
    }
    assert(result != nullptr && "Entity should always have look up entry");
    return result;
}
EntityLookUpEntry* GetEntityLookUpEntry(EntityStorage* storage, Entity* entity)
{
    return GetEntityLookUpEntry(storage, entity->m_Id);
}

Entity* GetEntity(EntityStorage* storage, EntityId id)
{
    EntityLookUpEntry* lookupEntry = GetEntityLookUpEntry(storage, id);
    Entity* result = storage->m_Entities + lookupEntry->m_StorageIndex;

    return result;
}

void InitializeEntiityStorage(EntityStorage* storage)
{
    storage->m_NextEntityId.m_Value = 1;
    storage->m_NumEntities = 0;
    //TODO probably clear hash
}

Entity* AddEntity(EntityStorage* storage)
{
    assert(storage->m_NumEntities < MAX_ENTITIES_NUM);
   
    Entity* result = storage->m_Entities + storage->m_NumEntities;
    result->m_Id = storage->m_NextEntityId;

    //probably should replace this with open adressing, need to check both with different patterns of spawning/unspawnin entities
    uint32_t currentIndex = EntityIdHash(result->m_Id);
    bool found = false;
    for (uint32_t i = 0; i < MAX_ENTITIES_NUM; ++i)
    {
        EntityLookUpEntry* entry = storage->m_EntityHash + currentIndex;
        if (entry->m_Id == INVALID_ENTITY_ID)
        {
            entry->m_Id = result->m_Id;
            entry->m_StorageIndex = storage->m_NumEntities;
            found = true;
            break;
        }
        else
        {
            currentIndex = (currentIndex + 1) % MAX_ENTITIES_NUM;
        }
    }

    if (found)
    {
        ++storage->m_NumEntities;
        ++storage->m_NextEntityId.m_Value;
    }
    else
    {
        //NOTE don't know yet if it's valid situation
        assert(false);
        result = nullptr;
    }

    return result;
}

void RemoveEntity(EntityStorage* storage, Entity* entity)
{  
    RemoveEntity(storage, entity->m_Id);
}

void RemoveEntity(EntityStorage* storage, EntityId entityId)
{
    EntityLookUpEntry* deletedEntry = GetEntityLookUpEntry(storage, entityId);
    Entity* deletedEntity = storage->m_Entities + deletedEntry->m_StorageIndex;

    Entity* lastEntity = storage->m_Entities + (storage->m_NumEntities - 1);
    EntityLookUpEntry* lastEntry = GetEntityLookUpEntry(storage, lastEntity);

    *deletedEntity = *lastEntity;
    lastEntry->m_StorageIndex = deletedEntry->m_StorageIndex;
    deletedEntry->m_Id = INVALID_ENTITY_ID;

    --storage->m_NumEntities;
}
