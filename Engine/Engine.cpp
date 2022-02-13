#include <cstdint>
#include <Engine/Engine.h>
#include <Engine/Engine_platform.h>
#include <Engine/Bitmap.h>
#include <Math/Vector2.hpp>

GameMemory* g_DebugGlobalMemory;

GameState* GetGameState(GameMemory* fromMem)
{
    return static_cast<GameState*>(fromMem->m_Memory);
}

void Initialize(GameMemory* gameMemory)
{
    g_DebugGlobalMemory = gameMemory;

    assert(sizeof(GameState) <= gameMemory->m_MemorySize);
    GameState* gameState = GetGameState(gameMemory);

    uint8_t* memoryBlockStart = (uint8_t*)gameMemory->m_Memory + sizeof(GameState);
    uint8_t* memoryBlockEnd = (uint8_t*)gameMemory->m_Memory + gameMemory->m_MemorySize;

    memoryBlockStart += (uint64_t)memoryBlockStart % 8; //align to 8 byte, 8 picked randomly to not foghet that this comes unaligned

    uint64_t memoryBlockTotalMemory = memoryBlockEnd - memoryBlockStart;

    gameState->m_TotalMemory.Initialize(memoryBlockStart, memoryBlockTotalMemory);
    gameState->m_CurrentWorld = CreateWorld(&gameState->m_TotalMemory, Vector3{ 500.0f, 500.0f, 500.0f });

    gameState->m_Simulation = CreateSimulation(&gameState->m_TotalMemory, WorldPosition{}, Vector3{ 1100.0f, 1100.0f, 1000.0f });

    gameState->m_Camera.m_SimPos = { 0.0f, 0.0f, 0.0f };
    gameState->m_Camera.m_WorldPos = ZeroPos();

    uint32_t persistentIndex = 0;

    Entity player;
    player.m_PersistentID.m_Value = ++persistentIndex;
    player.m_SimPos = { 0, 0, 0 };
    player.m_WorldPos = FitIntoChunk({ { 0, 0, 0 }, player.m_SimPos }, gameState->m_CurrentWorld->m_ChunkDimMeteres);
    player.m_Velocity = {};
    player.m_Size = { 3, 3 };
    player.m_Color = { 1, 0, 0};
    player.m_AngleRad = 0.0f;
    PackEntity(gameState->m_CurrentWorld, &player, player.m_WorldPos);

    Vector2 initObsPos = { 10, 10 };
    for (uint32_t i = 0; i < 2000; ++i)
    {
        for (uint32_t j = 0; j < 2000; ++j)
        {
            Entity obstacle;
            obstacle.m_PersistentID.m_Value = ++persistentIndex;
            obstacle.m_SimPos.xy = initObsPos + Vector2{ i * 50.0f, j * 50.0f };
            obstacle.m_SimPos.z = 0.0f;
            obstacle.m_Velocity = {};
            obstacle.m_Size = { (float)((i + 5) % 5) + 1.0f, (float)((j + 5) % 5)+ 1.0f };
            obstacle.m_Color = { 1, 1, 0 };
            obstacle.m_AngleRad = (float)((i + j + 5) % 5) * 0.1f;
            obstacle.m_WorldPos = FitIntoChunk({ { 0, 0, 0 }, obstacle.m_SimPos }, gameState->m_CurrentWorld->m_ChunkDimMeteres);

            PackEntity(gameState->m_CurrentWorld, &obstacle, obstacle.m_WorldPos);
        }
    }

    gameState->m_TestBMP = DEBUGLoadBMP(gameMemory->m_DEBUGReadFile, "Assets/Test.bmp");
}

void UpdateAndRender(float dt, GameMemory* gameMemory, GameInput* gameInput, EngineOffScreenBuffer* outBuffer)
{
    g_DebugGlobalMemory = gameMemory;

    GameState* gameState = GetGameState(gameMemory);
    Simulation* simulation = gameState->m_Simulation;
    World* world = gameState->m_CurrentWorld;

    UpdateSimulation(simulation, world, gameState->m_Camera.m_WorldPos);

    Vector3 playerAcceleration = {
        gameInput->m_ControllersInput[0].m_MoveAxisX + gameInput->m_KeyboardMouseController.m_MoveAxisX,
        gameInput->m_ControllersInput[0].m_MoveAxisY + gameInput->m_KeyboardMouseController.m_MoveAxisY,
        0.0f
    };

    if (LengthSq(playerAcceleration) > 1.0f)
    {
        playerAcceleration = GetNormilized(playerAcceleration);
    }

    //Entity* controlledEntity = GetEntity(&world->m_Entities, gameState->m_ControlledEntityId);

    //const float playerSpeed = 100.0f;
    //playerAcceleration *= playerSpeed;
    //playerAcceleration += -1.5f * controlledEntity->m_Velocity;

    //controlledEntity->m_SimPos
    //    += 0.5f * playerAcceleration * dt * dt
    //    + controlledEntity->m_Velocity * dt;

    //controlledEntity->m_Velocity += playerAcceleration * dt;

    //gameState->m_Camera.m_SimPos = controlledEntity->m_SimPos;

    ClearBuffer(outBuffer, Color{ 0.1f, 0.1f, 0.1f });

    gameState->m_Camera.m_SimPos = ToSimPos(gameState->m_Camera.m_WorldPos, simulation->m_Origin, world->m_ChunkDimMeteres);
    static Vector3 offset = Vector3{ 0.25f, 0.25f, 0.0f };
    if (simulation->m_Entities.m_NumEntities == 0)
    {
        offset = -1.0f * offset;
    }
    gameState->m_Camera.m_SimPos += offset;

    gameState->m_Camera.m_WorldPos = ToWorldPos(simulation->m_Origin, gameState->m_Camera.m_SimPos, world->m_ChunkDimMeteres);   

    for (uint32_t entityIndex = 0; entityIndex < simulation->m_Entities.m_NumEntities; ++entityIndex)
    {
        Entity* entity = simulation->m_Entities.m_Entities + entityIndex;

        entity->m_SimPos = ToSimPos(entity->m_WorldPos, simulation->m_Origin, world->m_ChunkDimMeteres);

        //do shit here in sim pos

        entity->m_WorldPos = ToWorldPos(simulation->m_Origin, entity->m_SimPos, world->m_ChunkDimMeteres);

        Vector2 worldObjectPosCameraSpace = (entity->m_SimPos - gameState->m_Camera.m_SimPos).xy;

        Vector2 screenOffset = { outBuffer->m_Width * 0.5f, outBuffer->m_Height * 0.5f };
#if 0

        Vector2 worldObjectLeftBottom = worldObjectPosCameraSpace;
        Vector2 worldObjectRightUp = worldObjectPosCameraSpace;

        worldObjectLeftBottom -= entity->m_Size.xy * 0.5f;
        worldObjectRightUp += entity->m_Size.xy * 0.5f;

        worldObjectLeftBottom *= PIXELS_IN_METRE;
        worldObjectRightUp *= PIXELS_IN_METRE;

        worldObjectLeftBottom += screenOffset;
        worldObjectRightUp += screenOffset;

        DrawRectangle(outBuffer, worldObjectLeftBottom, worldObjectRightUp, entity->m_Color);
#else
        Vector2 origin = worldObjectPosCameraSpace;
        origin *= PIXELS_IN_METRE;
        origin += screenOffset;

        Vector2 xAxis = Vector2{ cosf(entity->m_AngleRad), sinf(entity->m_AngleRad) };
        Vector2 yAxis = { -xAxis.y, xAxis.x };

        xAxis *= entity->m_Size.x * PIXELS_IN_METRE;
        yAxis *= entity->m_Size.y * PIXELS_IN_METRE;

        origin -= (yAxis * 0.5f);
        origin -= (xAxis * 0.5f);

        DrawBitmap(outBuffer, &gameState->m_TestBMP, origin, xAxis, yAxis);
#endif
    }


}