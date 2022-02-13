#pragma once

#include <cstdint>
#include <Engine/WorldPosition.h>
#include <Math/Vector4.hpp>

constexpr uint32_t MAX_ENTITIES_NUM = 15000;

struct EntityId
{
    uint32_t m_Value;

    bool operator==(EntityId other) const { return m_Value == other.m_Value; }
    bool operator!=(EntityId other) const { return m_Value != other.m_Value; }
};
constexpr EntityId INVALID_ENTITY_ID = { 0 };

struct Entity
{
   EntityId m_Id;
   EntityId m_PersistentID;
   WorldPosition m_WorldPos;
   Vector3 m_SimPos;
   Vector4 m_Color;
   Vector3 m_Velocity;
   Vector3 m_Size;
   float m_AngleRad;
};

struct EntityLookUpEntry
{
    EntityId m_Id;
    uint32_t m_StorageIndex;
};

struct EntityStorage
{
    // NOTE: set of free ids might be better as it will eliminate need of hash and will allow direct mapping
    // from m_EntityHash array by id
    EntityId m_NextEntityId;
    uint32_t m_NumEntities;
    Entity m_Entities[MAX_ENTITIES_NUM];

    EntityLookUpEntry m_EntityHash[MAX_ENTITIES_NUM];
};

void InitializeEntiityStorage(EntityStorage* storage);
Entity* AddEntity(EntityStorage* storage);
void RemoveEntity(EntityStorage* storage, Entity* entity);
void RemoveEntity(EntityStorage* storage, EntityId entity);
Entity* GetEntity(EntityStorage* storage, EntityId id);

//#TODO
// World devided by chunks
// each chunk contains list of all entities
// chunks that are near to the camera are streamed in, streamed out otherwise
// decide if all entities should be streamed in/out every frame or just chunks that are in/out
// should there be two separate indexes, one persistent for streamed out entities and one transiont that is currently in simulation?
// if one entity is referencing another should it do it by persistent index or if enetity is streamed out - just forget about it
// or make it "hard ref" - do not stream out referenced entities
// should we use doubles for world position or indecies + float and remap them to float in camera space