#pragma once
#include <Windows.h>
#include <Engine/Engine.h>

struct Win32OffScreenBuffer
{
    BITMAPINFO m_Info = {};
    void* m_Memory = NULL;
    int m_Width = 0;
    int m_Height = 0;
    int m_BytesPerPixel = 0;
};

struct Win32WindowDimension
{
    int m_Width;
    int m_Height;
};

struct Win32GameCode
{
    HMODULE m_Module;
    FILETIME m_LastWriteTime;
    UpdateAndRenderSignature* UpdateAndRender;
};