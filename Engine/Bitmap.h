#pragma once

#include <Engine/Engine_platform.h>

#pragma pack(push, 1)
struct BitmapHeader
{
    uint16_t FileType;
    uint32_t FileSize;
    uint16_t Reserved1;
    uint16_t Reserved2;
    uint32_t BitmapOffset;
    uint32_t Size;
    int32_t Width;
    int32_t Height;
    uint16_t Planes;
    uint16_t BitsPerPixel;
    uint32_t Compression;
    uint32_t SizeOfBitmap;
    int32_t HorzResolution;
    int32_t VertResolution;
    uint32_t ColorsUsed;
    uint32_t ColorsImportant;

    uint32_t RedMask;
    uint32_t GreenMask;
    uint32_t BlueMask;
};
#pragma pack(pop)

struct LoadedBitmap
{
    uint32_t Width;
    uint32_t Height;
    uint32_t Pitch;
    void *Memory;
};

LoadedBitmap DEBUGLoadBMP(DEBUGReadEntireFileSignature* debugReadFile, char* fileName);