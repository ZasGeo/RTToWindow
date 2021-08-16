#pragma once
#include <Windows.h>
#include <Engine/Engine_platform.h>

struct Win32OffScreenBuffer
{
    BITMAPINFO m_Info;
    void* m_Memory;
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_BytesPerPixel;
    uint32_t m_Pitch;
    float m_AspectRatio;
};

struct Win32WindowDimension
{
    uint32_t m_Width;
    uint32_t m_Height;
};

struct Win32GameCode
{
    HMODULE m_Module;
    FILETIME m_LastWriteTime;
    UpdateAndRenderSignature* UpdateAndRender;
    InitializeSignature* Initialize;
};