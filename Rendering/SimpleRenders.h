#pragma once

#include <Engine/Engine.h>

struct Rectangle
{
    float startX;
    float startY;
    float endX;
    float endY;
};

struct Color
{
    float R;
    float G;
    float B;
};

void RenderGradient(EngineOffScreenBuffer* outBuffer);
void ClearBuffer(EngineOffScreenBuffer* outBuffer, Color color);
void DrawRectangle(EngineOffScreenBuffer* outBuffer, Rectangle rect, Color color);