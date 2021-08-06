#pragma once
#include <Windows.h>
#include <Engine/Engine.h>

struct Win32OffScreenBuffer
{
    BITMAPINFO m_Info = {};
    void* m_Memory = NULL;
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_BytesPerPixel = 0;
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