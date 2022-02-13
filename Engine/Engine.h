#pragma once

#include <Engine/Entity.h>
#include <Engine/Bitmap.h>
#include <Engine/Memory.h>
#include <Engine/World.h>
#include <Engine/Simulation.h>
#include <Math/Vector3.hpp>
#include <Rendering/SimpleRenders.h>

constexpr float PIXELS_IN_METRE = 1.0f;

struct Camera
{
    WorldPosition m_WorldPos;
    Vector3 m_SimPos;
};

struct GameState
{
    World* m_CurrentWorld;
    Simulation* m_Simulation;

    Camera m_Camera;

    EntityId m_ControlledEntityId;

    LoadedBitmap m_TestBMP;

    MemoryBlock m_TotalMemory;
};