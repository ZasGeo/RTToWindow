#include <cstdint>
#include <Engine/Engine.h>
#include <Engine/Engine_platform.h>
#include <Rendering/SimpleRenders.h>
#include <Math/Vector2.hpp>

GameState* GetGameState(GameMemory* fromMem)
{
    return static_cast<GameState*>(fromMem->m_PersistentStorage);
}

void Initialize(GameMemory* gameMemory)
{
    GameState* gameState = GetGameState(gameMemory);

    uint8_t* linearAllocatorBase = (uint8_t*)gameMemory->m_PersistentStorage + sizeof(GameState);
    uint64_t linearAllocatorTotalMem = gameMemory->m_PersistentMemorySize - sizeof(GameState);
    gameState->m_LinearAllocator.Initialize(linearAllocatorBase, linearAllocatorTotalMem);

    gameState->m_Camera.m_Pos = { -48.0f, -27.0f };

    gameState->m_Player.m_Pos = { 0, 0 };
    gameState->m_Player.m_Velocity = {};

    gameState->m_Obstacle = { 10, 10 };

}

void UpdateAndRender(float dt, GameMemory* gameMemory, GameInput* gameInput, EngineOffScreenBuffer* outBuffer)
{
    GameState* gameState = GetGameState(gameMemory);

    Vector2 playerAcceleration = {
        gameInput->m_ControllersInput[0].m_MoveAxisX + gameInput->m_KeyboardMouseController.m_MoveAxisX,
        gameInput->m_ControllersInput[0].m_MoveAxisY + gameInput->m_KeyboardMouseController.m_MoveAxisY };

    const float playerSpeed = 20.0f;
    playerAcceleration *= playerSpeed;
    playerAcceleration += -2.0f * gameState->m_Player.m_Velocity;

    gameState->m_Player.m_Pos
        = 0.5f * playerAcceleration * dt * dt
        + gameState->m_Player.m_Velocity * dt
        + gameState->m_Player.m_Pos;

    gameState->m_Player.m_Velocity += playerAcceleration * dt;

    ClearBuffer(outBuffer, Color{ 0.1f, 0.1f, 0.1f });

    Vector2 playerPosCameraSpace = gameState->m_Player.m_Pos - gameState->m_Camera.m_Pos;
    if (playerPosCameraSpace.x > 96.0f)
    {
        gameState->m_Camera.m_Pos.x += 96.0f;
    }
    else if (playerPosCameraSpace.x < -0.0f)
    {
        gameState->m_Camera.m_Pos.x -= 96.0f;
    }
    if (playerPosCameraSpace.y > 54.f)
    {
        gameState->m_Camera.m_Pos.y += 54.0f;
    }
    else if (playerPosCameraSpace.y < 0.0f)
    {
        gameState->m_Camera.m_Pos.y -= 54.0f;
    }
    
    {
        Vector2 obstaclePosCameraSpace = gameState->m_Obstacle - gameState->m_Camera.m_Pos;

        Vector2 obsacleLeftBottom{ -3.0f, -3.0f };
        Vector2 obstacleRightUp{ 3.0f, 3.0f };
        obsacleLeftBottom += obstaclePosCameraSpace;
        obstacleRightUp += obstaclePosCameraSpace;

        obsacleLeftBottom *= PIXELS_IN_METRE;
        obstacleRightUp *= PIXELS_IN_METRE;

        DrawRectangle(outBuffer, obsacleLeftBottom, obstacleRightUp, Color{ 0.0f, 1.0f, 0.0f });
    }

    {
        playerPosCameraSpace = gameState->m_Player.m_Pos - gameState->m_Camera.m_Pos;
        Vector2 playerLeftBottom{ -1.0f, -1.0f };
        Vector2 playerRightUp{ 1.0f, 1.0f };
        playerLeftBottom += playerPosCameraSpace;
        playerRightUp += playerPosCameraSpace;

        playerLeftBottom *= PIXELS_IN_METRE;
        playerRightUp *= PIXELS_IN_METRE;

        DrawRectangle(outBuffer, playerLeftBottom, playerRightUp, Color{ 1.0f, 0.0f, 0.0f });
    }
}