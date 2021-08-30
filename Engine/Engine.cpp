#include <cstdint>
#include <Engine/Engine.h>
#include <Engine/Engine_platform.h>
#include <Math/Vector2.hpp>

GameState* GetGameState(GameMemory* fromMem)
{
    return static_cast<GameState*>(fromMem->m_PersistentStorage);
}

void Initialize(GameMemory* gameMemory)
{
    assert(sizeof(GameState) <= gameMemory->m_PersistentMemorySize);
    GameState* gameState = GetGameState(gameMemory);

    uint8_t* linearAllocatorBase = (uint8_t*)gameMemory->m_PersistentStorage + sizeof(GameState);
    uint64_t linearAllocatorTotalMem = gameMemory->m_PersistentMemorySize - sizeof(GameState);
    gameState->m_LinearAllocator.Initialize(linearAllocatorBase, linearAllocatorTotalMem);
    InitializeEntiityStorage(&gameState->m_World.m_Entities);

    gameState->m_World.m_Camera.m_Pos = { -48.0f, -27.0f };
    Entity* player = AddEntity(&gameState->m_World.m_Entities);
    
    player->m_Pos = { 0, 0 };
    player->m_Velocity = {};
    player->m_Size = { 3, 3 };
    player->m_Color = { 1, 0, 0};
    gameState->m_ControlledEntityId = player->m_Id;
    
    Vector2 initObsPos = { 10, 10 };
    for (uint32_t i = 0; i < 100; ++i)
    {
        for (uint32_t j = 0; j < 100; ++j)
        {
            Entity* obstacle = AddEntity(&gameState->m_World.m_Entities);
            obstacle->m_Pos.xy = initObsPos + Vector2{ i * 20.0f, j * 15.0f };
            obstacle->m_Velocity = {};
            obstacle->m_Size = { (float)((i + 5) % 5) + 1.0f, (float)((j + 5) % 5)+ 1.0f };
            obstacle->m_Color = { 1, 1, 0 };
        }
    }
}

void UpdateAndRender(float dt, GameMemory* gameMemory, GameInput* gameInput, EngineOffScreenBuffer* outBuffer)
{
    GameState* gameState = GetGameState(gameMemory);
    World* world = &gameState->m_World;
    Vector3 playerAcceleration = {
        gameInput->m_ControllersInput[0].m_MoveAxisX + gameInput->m_KeyboardMouseController.m_MoveAxisX,
        gameInput->m_ControllersInput[0].m_MoveAxisY + gameInput->m_KeyboardMouseController.m_MoveAxisY,
        0.0f
    };

    if (LengthSq(playerAcceleration) > 1.0f)
    {
        playerAcceleration = GetNormilized(playerAcceleration);
    }

    Entity* controlledEntity = GetEntity(&world->m_Entities, gameState->m_ControlledEntityId);

    const float playerSpeed = 100.0f;
    playerAcceleration *= playerSpeed;
    playerAcceleration += -1.5f * controlledEntity->m_Velocity;

    controlledEntity->m_Pos
        += 0.5f * playerAcceleration * dt * dt
        + controlledEntity->m_Velocity * dt;

    controlledEntity->m_Velocity += playerAcceleration * dt;

    world->m_Camera.m_Pos = controlledEntity->m_Pos;

    ClearBuffer(outBuffer, Color{ 0.1f, 0.1f, 0.1f });
    
    for (uint32_t entityIndex = 0; entityIndex < world->m_Entities.m_NumEntities; ++entityIndex)
    {
        Entity* entity = world->m_Entities.m_Entities + entityIndex;
        Vector2 worldObjectPosCameraSpace = (entity->m_Pos - world->m_Camera.m_Pos).xy;

        Vector2 worldObjectLeftBottom = worldObjectPosCameraSpace;
        Vector2 worldObjectRightUp = worldObjectPosCameraSpace;

        worldObjectLeftBottom -= entity->m_Size.xy * 0.5f;
        worldObjectRightUp += entity->m_Size.xy * 0.5f;

        worldObjectLeftBottom *= PIXELS_IN_METRE;
        worldObjectRightUp *= PIXELS_IN_METRE;

        Vector2 screenOffset = { outBuffer->m_Width * 0.5f, outBuffer->m_Height * 0.5f };
        worldObjectLeftBottom += screenOffset;
        worldObjectRightUp += screenOffset;

        DrawRectangle(outBuffer, worldObjectLeftBottom, worldObjectRightUp, entity->m_Color);
    }
}