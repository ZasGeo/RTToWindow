#include <Rendering/SimpleRenders.h>
#include <Utils/Utils.hpp>

uint32_t ColorToColorCode(Color color)
{
    return ((Utils::RoundFloatUint32(color.R * 255.0f) << 16) |
            (Utils::RoundFloatUint32(color.G * 255.0f) << 8) |
            (Utils::RoundFloatUint32(color.B * 255.0f) << 0));
}

void RenderGradient(EngineOffScreenBuffer* outBuffer)
{
    const int pitch = outBuffer->m_Width * outBuffer->m_BytesPerPixel;
    uint8_t* row = static_cast<uint8_t*>(outBuffer->m_Memory);
    for (uint32_t rowIndex = 0; rowIndex < outBuffer->m_Height; ++rowIndex)
    {
        uint32_t* pixel = reinterpret_cast<uint32_t*>(row);
        for (uint32_t columnIndex = 0; columnIndex < outBuffer->m_Width; ++columnIndex)
        {
            const uint8_t blue = static_cast<uint8_t>(rowIndex);
            const uint8_t green = static_cast<uint8_t>(columnIndex);
            *(pixel++) = green << 8 | blue;
        }

        row += pitch;
    }
}

void ClearBuffer(EngineOffScreenBuffer* outBuffer, Color color)
{
    const uint32_t colorCode = ColorToColorCode(color);
    const uint32_t pitch = outBuffer->m_Width * outBuffer->m_BytesPerPixel;
    uint8_t* row = static_cast<uint8_t*>(outBuffer->m_Memory);
    for (uint32_t rowIndex = 0; rowIndex < outBuffer->m_Height; ++rowIndex)
    {
        uint32_t* pixel = reinterpret_cast<uint32_t*>(row);
        for (uint32_t columnIndex = 0; columnIndex < outBuffer->m_Width; ++columnIndex)
        {
            *(pixel++) = colorCode;
        }

        row += pitch;
    }
}

void DrawRectangle(EngineOffScreenBuffer* outBuffer, Rectangle rect, Color color)
{
    const int32_t width = outBuffer->m_Width;
    const int32_t height = outBuffer->m_Height;
    const int32_t pitch = width * outBuffer->m_BytesPerPixel;
    const uint32_t colorCode = ColorToColorCode(color);

    const int32_t minX = Utils::GetMax(Utils::RoundFloatInt32(rect.startX), 0);
    const int32_t minY = Utils::GetMax(Utils::RoundFloatInt32(rect.startY), 0);
    const int32_t maxX = Utils::GetMin(Utils::RoundFloatInt32(rect.endX), width);
    const int32_t maxY = Utils::GetMin(Utils::RoundFloatInt32(rect.endY), height);
 
    for (int32_t rowIndex = minY; rowIndex < maxY; ++rowIndex)
    {
        uint32_t* row = static_cast<uint32_t*>(outBuffer->m_Memory) + rowIndex * width;
        for (int32_t columnIndex = minX; columnIndex < maxX; ++columnIndex)
        {
            uint32_t* pixel = row + columnIndex;
            *pixel = colorCode;
        }
    }
}
