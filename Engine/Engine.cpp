#include <cstdint>
#include <Engine/Engine.h>
#include <Rendering/SimpleRenders.h>

void Initialize(GameMemory* gameMemory)
{
    GameState* dummy = (GameState*)(gameMemory->m_PersistentStorage);
    dummy->m_xOffset = 0;
    dummy->m_yOffset = 0;
}

void UpdateAndRender(GameMemory* gameMemory, GameInput* gameInput, EngineOffScreenBuffer* outBuffer)
{
    GameState* dummy = (GameState*)(gameMemory->m_PersistentStorage);
    dummy->m_xOffset += 2.0f * gameInput->m_ControllersInput[0].m_MoveAxisX;
    dummy->m_yOffset += 2.0f * gameInput->m_ControllersInput[0].m_MoveAxisY;

    dummy->m_xOffset += 2.0f * gameInput->m_KeyboardMouseController.m_MoveAxisX;
    dummy->m_yOffset += 2.0f * gameInput->m_KeyboardMouseController.m_MoveAxisY;

    ClearBuffer(outBuffer, Color{ 0.0f, 0.0f, 0.0f });

    Rectangle rectPos;
    rectPos.startX = 300 + dummy->m_xOffset;
    rectPos.startY = 300 + dummy->m_yOffset;
    rectPos.endX = 400 + dummy->m_xOffset;
    rectPos.endY = 400 + dummy->m_yOffset;

    DrawRectangle(outBuffer, rectPos, Color{ 1.0f, 0.0f, 0.0f });
}