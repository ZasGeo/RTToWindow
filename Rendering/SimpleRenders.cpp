#include <Rendering/SimpleRenders.h>

void RenderGradient(EngineOffScreenBuffer* outBuffer)
{
    const int pitch = outBuffer->m_Width * outBuffer->m_BytesPerPixel;
    uint8_t* row = static_cast<uint8_t*>(outBuffer->m_Memory);
    for (int rowIndex = 0; rowIndex < outBuffer->m_Height; ++rowIndex)
    {
        uint32_t* pixel = reinterpret_cast<uint32_t*>(row);
        for (int columnIndex = 0; columnIndex < outBuffer->m_Width; ++columnIndex)
        {
            const uint8_t blue = static_cast<uint8_t>(rowIndex);
            const uint8_t green = static_cast<uint8_t>(columnIndex);
            *(pixel++) = green << 8 | blue;
        }

        row += pitch;
    }
}

void ClearBuffer(EngineOffScreenBuffer* outBuffer, uint32_t color)
{
    const int pitch = outBuffer->m_Width * outBuffer->m_BytesPerPixel;
    uint8_t* row = static_cast<uint8_t*>(outBuffer->m_Memory);
    for (int rowIndex = 0; rowIndex < outBuffer->m_Height; ++rowIndex)
    {
        uint32_t* pixel = reinterpret_cast<uint32_t*>(row);
        for (int columnIndex = 0; columnIndex < outBuffer->m_Width; ++columnIndex)
        {
            *(pixel++) = color;
        }

        row += pitch;
    }
}

void DrawRectangle(EngineOffScreenBuffer* outBuffer, float startX, float startY, float endX, float endY, uint32_t color)
{
    const int width = outBuffer->m_Width;
    const int height = outBuffer->m_Height;
    const int pitch = width * outBuffer->m_BytesPerPixel;

    int minX = static_cast<int>(startX + 0.5f);
    int minY = static_cast<int>(startY + 0.5f);
    int maxX = static_cast<int>(endX + 0.5f);
    int maxY = static_cast<int>(endY + 0.5f);

    minX = minX >= 0 ? minX : 0;
    minY = minY >= 0 ? minY : 0;

    maxX = maxX <= width ? maxX : width;
    maxY = maxY <= height ? maxY : height;

    
    for (int rowIndex = minY; rowIndex < maxY; ++rowIndex)
    {
        uint32_t* row = static_cast<uint32_t*>(outBuffer->m_Memory) + rowIndex * width;
        for (int columnIndex = minX; columnIndex < maxX; ++columnIndex)
        {
            uint32_t* pixel = row + columnIndex;
            *pixel = color;
        }
    }
}
