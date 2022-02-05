#pragma once

#include <cstdint>
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

   //payload
   Vector4 m_Color;
   Vector3 m_Pos;
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