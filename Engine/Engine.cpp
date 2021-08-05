#include "Engine.h"
#include <cstdint>

static int g_XOffset;
static int g_YOffset;

void RenderGradient(EngineOffScreenBuffer* outBuffer)
{
    uint8_t* row = static_cast<uint8_t*>(outBuffer->m_Memory);
    for (int rowIndex = 0; rowIndex < outBuffer->m_Height; ++rowIndex)
    {
        uint32_t* pixel = reinterpret_cast<uint32_t*>(row);
        for (int columnIndex = 0; columnIndex < outBuffer->m_Width; ++columnIndex)
        {
            const uint8_t blue = static_cast<uint8_t>(rowIndex + g_YOffset);
            const uint8_t green = static_cast<uint8_t>(columnIndex + g_XOffset);
            *(pixel++) = green << 8 | blue;
        }

        row += outBuffer->m_Width * outBuffer->m_BytesPerPixel;
    }
}

void UpdateAndRender(GameMemory* gameMemory, GameInput* gameInput, EngineOffScreenBuffer* outBuffer)
{
    g_XOffset += static_cast<int>(4 * gameInput->m_ControllersInput[0].m_MoveAxisX);
    g_YOffset += static_cast < int>(4 * gameInput->m_ControllersInput[0].m_MoveAxisY);

    g_XOffset += static_cast<int>(4 * gameInput->m_KeyboardMouseController.m_MoveAxisX);
    g_YOffset += static_cast <int>(4 * gameInput->m_KeyboardMouseController.m_MoveAxisY);

    g_XOffset += static_cast<int>(4 * gameInput->m_KeyboardMouseController.m_TurnAxisX);
    g_YOffset += static_cast <int>(4 * gameInput->m_KeyboardMouseController.m_TurnAxisY);

    RenderGradient(outBuffer);
}