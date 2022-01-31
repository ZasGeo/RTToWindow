#pragma once

#include <Engine/Engine_platform.h>
#include <Math/Vector4.hpp>
#include <Engine/Bitmap.h>

using Color = Vector4;

void RenderGradient(EngineOffScreenBuffer* outBuffer);
void ClearBuffer(EngineOffScreenBuffer* outBuffer, Vector4 color);
void DrawRectangle(EngineOffScreenBuffer* outBuffer, Vector2 leftBottom, Vector2 rightUp, Color color);
void DrawBitmap(EngineOffScreenBuffer* outBuffer, LoadedBitmap* bitmap);
void DrawBitmap(EngineOffScreenBuffer* outBuffer, LoadedBitmap* bitmap, Vector2 origin, Vector2 xAxis, Vector2 yAxis);