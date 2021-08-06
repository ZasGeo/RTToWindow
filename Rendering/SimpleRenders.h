#pragma once

#include <Engine/Engine.h>

void RenderGradient(EngineOffScreenBuffer* outBuffer);
void ClearBuffer(EngineOffScreenBuffer* outBuffer, uint32_t color);
void DrawRectangle(EngineOffScreenBuffer* outBuffer, float startX, float startY, float endX, float endY, uint32_t color);