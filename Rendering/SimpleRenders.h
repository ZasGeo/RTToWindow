#pragma once

#include <Engine/Engine_platform.h>
#include <Math/Vector4.hpp>

using Color = Vector4;

void RenderGradient(EngineOffScreenBuffer* outBuffer);
void ClearBuffer(EngineOffScreenBuffer* outBuffer, Vector4 color);
void DrawRectangle(EngineOffScreenBuffer* outBuffer, Vector2 leftBottom, Vector2 rightUp, Color color);