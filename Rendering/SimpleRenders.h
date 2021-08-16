#pragma once

#include <Engine/Engine_platform.h>
#include <Math/Vector2.hpp>


struct Color
{
    float R;
    float G;
    float B;
};

void RenderGradient(EngineOffScreenBuffer* outBuffer);
void ClearBuffer(EngineOffScreenBuffer* outBuffer, Color color);
void DrawRectangle(EngineOffScreenBuffer* outBuffer, Vector2 leftBottom, Vector2 rightUp, Color color);