#pragma once

#include <Engine/Allocators.h>
#include <Engine/Entity.h>
#include <Math/Vector3.hpp>
#include <Rendering/SimpleRenders.h>

constexpr float PIXELS_IN_METRE = 10.0f;

struct Camera
{
    Vector3 m_Pos;
};

struct World
{
    Camera m_Camera;
    EntityStorage m_Entities;
};

struct GameState
{
    LinearAllocator m_LinearAllocator;

    World m_World;
    EntityId m_ControlledEntityId;
};