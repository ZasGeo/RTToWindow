#include <Engine/Bitmap.h>
#include <Math/Utils.hpp>
#include <Math/Math.hpp>
#include <Math/Vector4.hpp>
#include <cassert>

LoadedBitmap DEBUGLoadBMP(DEBUGReadEntireFileSignature* debugReadFile, char* fileName)
{
	LoadedBitmap result = {};

    DebugReadFileResult readResult = debugReadFile(fileName);

    if (readResult.m_Size != 0)
    {
        BitmapHeader* header = (BitmapHeader*)readResult.m_Content;
        uint32_t* pixels = (uint32_t*)((uint8_t*)readResult.m_Content + header->BitmapOffset);
        result.Memory = pixels;
        result.Width = header->Width;
        result.Height = header->Height;

        assert(result.Height >= 0);
        assert(header->Compression == 3);

        // NOTE(casey): Byte order in memory is determined by the Header itself,
        // so we have to read out the masks and convert the pixels ourselves.
        uint32_t redMask = header->RedMask;
        uint32_t greenMask = header->GreenMask;
        uint32_t blueMask = header->BlueMask;
        uint32_t alphaMask = ~(redMask | greenMask | blueMask);

        BitScanResult redScan = FindLeastSignificantSetBit(redMask);
        BitScanResult greenScan = FindLeastSignificantSetBit(greenMask);
        BitScanResult blueScan = FindLeastSignificantSetBit(blueMask);
        BitScanResult alphaScan = FindLeastSignificantSetBit(alphaMask);

        assert(redScan.Found);
        assert(greenScan.Found);
        assert(blueScan.Found);
        assert(alphaScan.Found);

        int32_t redShiftDown = (int32_t)redScan.Index;
        int32_t greenShiftDown = (int32_t)greenScan.Index;
        int32_t blueShiftDown = (int32_t)blueScan.Index;
        int32_t alphaShiftDown = (int32_t)alphaScan.Index;

        uint32_t* sourceDest = pixels;
        for (int32_t Y = 0;
            Y < header->Height;
            ++Y)
        {
            for (int32_t X = 0;
                X < header->Width;
                ++X)
            {
                uint32_t color = *sourceDest;

                Vector4 Texel = { (float)((color & redMask) >> redShiftDown),
                            (float)((color & greenMask) >> greenShiftDown),
                            (float)((color & blueMask) >> blueShiftDown),
                            (float)((color & alphaMask) >> alphaShiftDown) };

                Texel = SRGB255ToLinear01(Texel);
                Texel.rgb *= Texel.a;
                Texel = Linear01ToSRGB255(Texel);

                *sourceDest++ = (((uint32_t)(Texel.a + 0.5f) << 24) |
                    ((uint32_t)(Texel.r + 0.5f) << 16) |
                    ((uint32_t)(Texel.g + 0.5f) << 8) |
                    ((uint32_t)(Texel.b + 0.5f) << 0));
            }
        }
    }

    result.Pitch = result.Width * 4;


	return result;
}