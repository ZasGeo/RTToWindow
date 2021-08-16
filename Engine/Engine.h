#pragma once

#include <Engine/Allocators.h>
#include <Math/Vector2.hpp>

constexpr float PIXELS_IN_METRE = 10.0f;

struct Camera
{
    Vector2 m_Pos;
};

struct Player
{
    Vector2 m_Pos;
    Vector2 m_Velocity;
};

struct GameState
{
    LinearAllocator m_LinearAllocator;
    Camera m_Camera;
    Player m_Player;
    Vector2 m_Obstacle;
};