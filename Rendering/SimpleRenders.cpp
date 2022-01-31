#include <Rendering/SimpleRenders.h>
#include <Math/Utils.hpp>
#include <Math/Math.hpp>

uint32_t Pack01To255(Color color)
{
    return ((uint32_t)(color.a * 255.0f) << 24 |
            (uint32_t)(color.r * 255.0f) << 16 |
            (uint32_t)(color.g * 255.0f) << 8 |
            (uint32_t)(color.b * 255.0f) << 0);
}

uint32_t Pack255(Color color)
{
    return ((RoundFloatUint32(color.a) << 24) |
            (RoundFloatUint32(color.r) << 16) |
            (RoundFloatUint32(color.g) << 8) |
            (RoundFloatUint32(color.b) << 0));
}

Color Unpack255(uint32_t colorCode)
{
    Color result = { (float)((colorCode >> 16) & 0xFF),
                    (float)((colorCode >> 8) & 0xFF),
                    (float)((colorCode >> 0) & 0xFF),
                    (float)((colorCode >> 24) & 0xFF) };
    return result;
}

void RenderGradient(EngineOffScreenBuffer* outBuffer)
{
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

        row += outBuffer->m_Pitch;
    }
}

void ClearBuffer(EngineOffScreenBuffer* outBuffer, Color color)
{
    const uint32_t colorCode = Pack01To255(color);
  
    BEGIN_TIMED_BLOCK(ClearBuffer)

#if 0
    __m256i colorWide = _mm256_set1_epi32(colorCode);
    for (uint32_t* pixel = (uint32_t*)outBuffer->m_Memory; pixel != (uint32_t*)outBuffer->m_Memory + outBuffer->m_Height * outBuffer->m_Width; pixel += 8)
    {
        _mm256_store_si256((__m256i*)pixel, colorWide);
    }
#else
    uint8_t* row = static_cast<uint8_t*>(outBuffer->m_Memory);
    for (uint32_t rowIndex = 0; rowIndex < outBuffer->m_Height; ++rowIndex)
    {
        uint32_t* pixel = reinterpret_cast<uint32_t*>(row);
        for (uint32_t columnIndex = 0; columnIndex < outBuffer->m_Width; ++columnIndex)
        {
            *(pixel++) = colorCode;
        }

        row += outBuffer->m_Pitch;
    }
#endif

    END_TIMED_BLOCK(ClearBuffer)
}

void DrawRectangle(EngineOffScreenBuffer* outBuffer, Vector2 leftBottom, Vector2 rightUp, Color color)
{
    BEGIN_TIMED_BLOCK(DrawRect)

    const int32_t width = outBuffer->m_Width;
    const int32_t height = outBuffer->m_Height;
    const uint32_t colorCode = Pack01To255(color);

    const int32_t minX = GetMax(FloorFloatInt32(leftBottom.x), 0);
    const int32_t minY = GetMax(FloorFloatInt32(leftBottom.y), 0);
    const int32_t maxX = GetMin(FloorFloatInt32(rightUp.x), width);
    const int32_t maxY = GetMin(FloorFloatInt32(rightUp.y), height);
 
#if 1
    for (int64_t rowIndex = minY; rowIndex < maxY; ++rowIndex)
    {
        uint32_t* row = static_cast<uint32_t*>(outBuffer->m_Memory) + rowIndex * width;
        for (int32_t columnIndex = minX; columnIndex < maxX; ++columnIndex)
        {
            BEGIN_TIMED_BLOCK(DrawRectPixel)
            uint32_t* pixel = row + columnIndex;
            *pixel = colorCode;
            END_TIMED_BLOCK(DrawRectPixel)
        }
    }
#else

    __m256i colorWide = _mm256_set1_epi32(colorCode);

    __m256i byteCount = _mm256_set_epi32(1, 2, 3, 4, 5, 6, 7, 8);
    __m256i zero = _mm256_set1_epi32(0);

    for (int64_t rowIndex = minY; rowIndex < maxY; ++rowIndex)
    {
        uint32_t* row = static_cast<uint32_t*>(outBuffer->m_Memory) + rowIndex * width;
        uint32_t* rowBound = row + maxX;
        for (int32_t columnIndex = minX; columnIndex < maxX; columnIndex += 8)
        {
            BEGIN_TIMED_BLOCK(DrawRectPixel)
            uint32_t* pixel = row + columnIndex;
            __m256i outOfBound = _mm256_max_epi32(_mm256_set1_epi32((int)((pixel + 8) - rowBound)), zero);
            __m256i writeMask = _mm256_cmpgt_epi32(byteCount, outOfBound);

            __m256i originalData = _mm256_loadu_si256((__m256i*)pixel);

            __m256i maskedOut = _mm256_or_si256(_mm256_and_si256(writeMask, colorWide), _mm256_andnot_si256(writeMask, originalData));

            _mm256_storeu_si256((__m256i*)pixel, maskedOut);
            END_TIMED_BLOCK_COUNTED(DrawRectPixel, 8)
        }
    }
#endif

    END_TIMED_BLOCK(DrawRect)
}

void DrawBitmap(EngineOffScreenBuffer* outBuffer, LoadedBitmap* bitmap)
{
    for (uint32_t y = 0; y < bitmap->Height; y++)
    {
        for (uint32_t x = 0; x < bitmap->Width; x++)
        {
            uint32_t* dest = (uint32_t*)outBuffer->m_Memory + y * outBuffer->m_Width + x;
            uint32_t* src = (uint32_t*)bitmap->Memory + y * bitmap->Width + x;

            Vector4 texel = { (float)((*src >> 16) & 0xFF),
                            (float)((*src >> 8) & 0xFF),
                            (float)((*src >> 0) & 0xFF),
                            (float)((*src >> 24) & 0xFF) };

            texel = SRGB255ToLinear01(texel);

            Vector4 destTexel = { (float)((*dest >> 16) & 0xFF),
                                (float)((*dest >> 8) & 0xFF),
                                (float)((*dest >> 0) & 0xFF),
                                (float)((*dest >> 24) & 0xFF) };

            destTexel = SRGB255ToLinear01(destTexel);

            Vector4 result = (1.0f - texel.a) * destTexel + texel;

            result = Linear01ToSRGB255(result);

            *dest = (((uint32_t)(result.a + 0.5f) << 24) |
                    ((uint32_t)(result.r + 0.5f) << 16) |
                    ((uint32_t)(result.g + 0.5f) << 8) |
                    ((uint32_t)(result.b + 0.5f) << 0));
        }
    }
}

#if 0
void DrawBitmap(EngineOffScreenBuffer* outBuffer, LoadedBitmap* bitmap, Vector2 origin, Vector2 xAxis, Vector2 yAxis)
{
    BEGIN_TIMED_BLOCK(DrawBitmap)

    const int32_t widthMax = (int32_t)outBuffer->m_Width - 1;
    const int32_t heightMax = (int32_t)outBuffer->m_Height - 1;

    int32_t xMin = widthMax;
    int32_t xMax = 0;
    int32_t yMin = heightMax;
    int32_t yMax = 0;

    Vector2 drawBoundingPoints[4] = { origin, origin + xAxis, origin + xAxis + yAxis, origin + yAxis };
    for (uint32_t i = 0; i < 4; ++i)
    {
        Vector2 test = drawBoundingPoints[i];

        int32_t floorX = FloorFloatInt32(test.x);
        int32_t ceilX = CeilFloatInt32(test.x);
        int32_t floorY = FloorFloatInt32(test.y);
        int32_t ceilY = CeilFloatInt32(test.y);

        xMin = xMin > floorX ? floorX : xMin;
        yMin = yMin > floorY ? floorY : yMin;
        xMax = xMax < ceilX ? ceilX : xMax;
        yMax = yMax < ceilY ? ceilY : yMax;
    }

    xMin = xMin < 0 ? 0 : xMin;
    yMin = yMin < 0 ? 0 : yMin;
    xMax = xMax > widthMax ? widthMax : xMax;
    yMax = yMax > heightMax ? heightMax : yMax;

    const float invXAxisLenghtSq = 1.0f / LengthSq(xAxis);
    const float invYAxisLenghtSq = 1.0f / LengthSq(yAxis);

    uint8_t* row = (uint8_t*)outBuffer->m_Memory + yMin * outBuffer->m_Pitch + xMin * 4;

    for (int32_t y = yMin; y <= yMax; ++y)
    {
        uint32_t* pixel = (uint32_t*)(row);
        for (int32_t x = xMin; x <= xMax; ++x)
        {
            Vector2 pixelPos = { (float)x, (float)y };
            Vector2 distanceToPisel = pixelPos - origin;

            float U = Dot(distanceToPisel, xAxis) * invXAxisLenghtSq;
            float V = Dot(distanceToPisel, yAxis) * invYAxisLenghtSq;

            if (U >= 0.0f &&
                U <= 1.0f &&
                V >= 0.0f &&
                V <= 1.0f)
            {
                const float textureX = U * (float)(bitmap->Width - 2);
                const float textureY = V * (float)(bitmap->Height - 2);

                const uint32_t xSample = (uint32_t)(textureX);
                const uint32_t ySample = (uint32_t)(textureY);

                const float fX = textureX - (float)xSample;
                const float fY = textureY - (float)ySample;

                //bilinear sample
                uint8_t* texelPtr = ((uint8_t*)bitmap->Memory) + ySample * bitmap->Pitch + xSample * sizeof(uint32_t);
                uint32_t texelA = *(uint32_t*)(texelPtr);
                uint32_t texelB = *(uint32_t*)(texelPtr + sizeof(uint32_t));
                uint32_t texelC = *(uint32_t*)(texelPtr + bitmap->Pitch);
                uint32_t texelD = *(uint32_t*)(texelPtr + bitmap->Pitch + sizeof(uint32_t));

                //SRGB 0-255
                Color texelAUnpacked = Unpack255(texelA);
                Color texelBUnpacked = Unpack255(texelB);
                Color texelCUnpacked = Unpack255(texelC);
                Color texelDUnpacked = Unpack255(texelD);

                //Linear 0-1
                texelAUnpacked = SRGB255ToLinear01(texelAUnpacked);
                texelBUnpacked = SRGB255ToLinear01(texelBUnpacked);
                texelCUnpacked = SRGB255ToLinear01(texelCUnpacked);
                texelDUnpacked = SRGB255ToLinear01(texelDUnpacked);

                //Linear 0-1
                Color colorBlended = Lerp(Lerp(texelAUnpacked, texelBUnpacked, fX), Lerp(texelCUnpacked, texelDUnpacked, fX), fY);

                Color dest = Unpack255(*pixel);

                dest = SRGB255ToLinear01(dest);

                Color resulting = (1.0f - colorBlended.a) * dest + colorBlended;

                resulting = Linear01ToSRGB255(resulting);

                *pixel = Pack255(resulting);
            }

            ++pixel;
        }
        row += outBuffer->m_Pitch;
    }

    END_TIMED_BLOCK(DrawBitmap)
}
#elif 1
void DrawBitmap(EngineOffScreenBuffer* outBuffer, LoadedBitmap* bitmap, Vector2 origin, Vector2 xAxis, Vector2 yAxis)
{
    BEGIN_TIMED_BLOCK(DrawBitmap)

    const int32_t widthMax = (int32_t)outBuffer->m_Width - 1;
    const int32_t heightMax = (int32_t)outBuffer->m_Height - 1;

    int32_t xMin = widthMax;
    int32_t xMax = 0;
    int32_t yMin = heightMax;
    int32_t yMax = 0;

    Vector2 drawBoundingPoints[4] = { origin, origin + xAxis, origin + xAxis + yAxis, origin + yAxis };
    for (uint32_t i = 0; i < 4; ++i)
    {
        Vector2 test = drawBoundingPoints[i];

        int32_t floorX = FloorFloatInt32(test.x);
        int32_t ceilX = CeilFloatInt32(test.x);
        int32_t floorY = FloorFloatInt32(test.y);
        int32_t ceilY = CeilFloatInt32(test.y);

        xMin = xMin > floorX ? floorX : xMin;
        yMin = yMin > floorY ? floorY : yMin;
        xMax = xMax < ceilX ? ceilX : xMax;
        yMax = yMax < ceilY ? ceilY : yMax;
    }

    xMin = xMin < 0 ? 0 : xMin;
    yMin = yMin < 0 ? 0 : yMin;
    xMax = xMax > widthMax ? widthMax : xMax;
    yMax = yMax > heightMax ? heightMax : yMax;

    const float invXAxisLenghtSq = 1.0f / LengthSq(xAxis);
    const float invYAxisLenghtSq = 1.0f / LengthSq(yAxis);

    uint32_t xMinAdjusted = (xMin & ~7)  ? (xMin & ~7) : xMin;
    uint32_t xMaxAdjusted = (xMax & ~7) ? (xMax & ~7) + 8 : xMax;

    uint32_t texHeight = bitmap->Height;

    __m256 widthM2 = _mm256_set1_ps((float)(bitmap->Width - 2));
    __m256 heightM2 = _mm256_set1_ps((float)(bitmap->Height - 2));
    __m256i texWidth8x = _mm256_set1_epi32(bitmap->Width);

    __m256i zero = _mm256_set1_epi32(0);
    __m256i four = _mm256_set1_epi32(4);
    __m256 zeroReal = _mm256_set1_ps(0.0f);
    __m256 oneReal = _mm256_set1_ps(1.0f);
    __m256 Octo8x = _mm256_set1_ps(8.0f);
    __m256 half_8x = _mm256_set1_ps(0.5f);

    __m256 originX8x = _mm256_set1_ps(origin.x);
    __m256 originY8x = _mm256_set1_ps(origin.y);
    __m256 invXAxisLenghtSq8x = _mm256_set1_ps(invXAxisLenghtSq);
    __m256 invYAxisLenghtSq8x = _mm256_set1_ps(invYAxisLenghtSq);

    __m256 xAxisX8x = _mm256_set1_ps(xAxis.x);
    __m256 xAxisY8x = _mm256_set1_ps(xAxis.y);
    __m256 yAxisX8x = _mm256_set1_ps(yAxis.x);
    __m256 yAxisY8x = _mm256_set1_ps(yAxis.y);

    __m256i pixelMask = _mm256_set1_epi32(0x000000FF);
    __m256 one255_8x = _mm256_set1_ps(255.0f);
    __m256 inv255_8x = _mm256_set1_ps(1.0f / 255.0f);

    //seems like UV mask works fine alone
    /*__m256i byteCountStart = _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 8);
    __m256i byteCountEnd = _mm256_set_epi32(1, 2, 3, 4, 5, 6, 7, 8);

    __m256i outOfBoundStart = _mm256_max_epi32(_mm256_set1_epi32(xMin - xMinAdjusted), zero);
    __m256i outOfBoundEnd = _mm256_max_epi32(_mm256_set1_epi32(xMaxAdjusted - xMax), zero);
    __m256i startWriteMask = _mm256_cmpgt_epi32(byteCountStart, outOfBoundStart);
    __m256i endWriteMask = _mm256_cmpgt_epi32(byteCountEnd, outOfBoundEnd);*/

    xMin = xMinAdjusted;
    xMax = xMaxAdjusted;

    int* textureMemory = (int*)bitmap->Memory;
    uint32_t texturePitch = bitmap->Pitch;

    uint8_t* row = (uint8_t*)outBuffer->m_Memory + yMin * outBuffer->m_Pitch + xMin * 4;

    for (int32_t y = yMin; y <= yMax; ++y)
    {
        uint32_t* pixel = (uint32_t*)(row);

        __m256 pixelPosY = _mm256_set1_ps((float)y);
        __m256 pixelPosX = _mm256_setr_ps(
            float(xMin),
            float(xMin + 1),
            float(xMin + 2),
            float(xMin + 3),
            float(xMin + 4),
            float(xMin + 5),
            float(xMin + 6),
            float(xMin + 7));

        pixelPosY = _mm256_sub_ps(pixelPosY, originY8x);
        pixelPosX = _mm256_sub_ps(pixelPosX, originX8x);

        for (int32_t x = xMin; x < xMax; x += 8)
        {
            __m256i source = _mm256_load_si256((__m256i*)pixel);

            __m256 u = _mm256_mul_ps(_mm256_add_ps(_mm256_mul_ps(pixelPosX, xAxisX8x), _mm256_mul_ps(pixelPosY, xAxisY8x)), invXAxisLenghtSq8x);
            __m256 v = _mm256_mul_ps(_mm256_add_ps(_mm256_mul_ps(pixelPosX, yAxisX8x), _mm256_mul_ps(pixelPosY, yAxisY8x)), invYAxisLenghtSq8x);

            __m256i writeMask = _mm256_castps_si256(_mm256_and_ps(_mm256_and_ps(_mm256_cmp_ps(u, zeroReal, 13),
                                                                                _mm256_cmp_ps(u, oneReal, 2)),
                                                                  _mm256_and_ps(_mm256_cmp_ps(v, zeroReal, 13),
                                                                                _mm256_cmp_ps(v, oneReal, 2))));

            u = _mm256_min_ps(_mm256_max_ps(u, zeroReal), oneReal);
            v = _mm256_min_ps(_mm256_max_ps(v, zeroReal), oneReal);

            __m256 textureX = _mm256_add_ps(_mm256_mul_ps(u, widthM2), half_8x);
            __m256 textureY = _mm256_add_ps(_mm256_mul_ps(v, heightM2), half_8x);

            __m256i fetchX = _mm256_cvttps_epi32(textureX);
            __m256i fetchY = _mm256_cvttps_epi32(textureY);

            __m256i fetchCoumpond = _mm256_mullo_epi16(fetchY, texWidth8x);

            fetchCoumpond = _mm256_add_epi32(fetchX, fetchCoumpond);

            //BEGIN_TIMED_BLOCK(DrawBitmapFetch);
#if 0
            __m256i sampleA = _mm256_i32gather_epi32(textureMemory, fetchCoumpond, sizeof(int32_t));
            __m256i sampleB = _mm256_i32gather_epi32(textureMemory + 1, fetchCoumpond, sizeof(int32_t));
            __m256i sampleC = _mm256_i32gather_epi32(textureMemory + texHeight, fetchCoumpond, sizeof(int32_t));
            __m256i sampleD = _mm256_i32gather_epi32(textureMemory + texHeight + 1, fetchCoumpond, sizeof(int32_t));

#else
            fetchCoumpond = _mm256_mullo_epi16(fetchCoumpond, four);
            uint32_t fetch0 = fetchCoumpond.m256i_u32[0];
            uint32_t fetch1 = fetchCoumpond.m256i_u32[1];
            uint32_t fetch2 = fetchCoumpond.m256i_u32[2];
            uint32_t fetch3 = fetchCoumpond.m256i_u32[3];
            uint32_t fetch4 = fetchCoumpond.m256i_u32[4];
            uint32_t fetch5 = fetchCoumpond.m256i_u32[5];
            uint32_t fetch6 = fetchCoumpond.m256i_u32[6];
            uint32_t fetch7 = fetchCoumpond.m256i_u32[7];


            uint8_t* texelPtr0 = ((uint8_t*)textureMemory) + fetch0;
            uint8_t* texelPtr1 = ((uint8_t*)textureMemory) + fetch1;
            uint8_t* texelPtr2 = ((uint8_t*)textureMemory) + fetch2;
            uint8_t* texelPtr3 = ((uint8_t*)textureMemory) + fetch3;
            uint8_t* texelPtr4 = ((uint8_t*)textureMemory) + fetch4;
            uint8_t* texelPtr5 = ((uint8_t*)textureMemory) + fetch5;
            uint8_t* texelPtr6 = ((uint8_t*)textureMemory) + fetch6;
            uint8_t* texelPtr7 = ((uint8_t*)textureMemory) + fetch7;

            __m256i sampleA = _mm256_setr_epi32(
                *(uint32_t*)(texelPtr0),
                *(uint32_t*)(texelPtr1),
                *(uint32_t*)(texelPtr2),
                *(uint32_t*)(texelPtr3),
                *(uint32_t*)(texelPtr4),
                *(uint32_t*)(texelPtr5),
                *(uint32_t*)(texelPtr6),
                *(uint32_t*)(texelPtr7));

            __m256i sampleB = _mm256_setr_epi32(
                *(uint32_t*)(texelPtr0 + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr1 + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr2 + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr3 + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr4 + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr5 + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr6 + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr7 + sizeof(uint32_t)));

            __m256i sampleC = _mm256_setr_epi32(
                *(uint32_t*)(texelPtr0 + texturePitch),
                *(uint32_t*)(texelPtr1 + texturePitch),
                *(uint32_t*)(texelPtr2 + texturePitch),
                *(uint32_t*)(texelPtr3 + texturePitch),
                *(uint32_t*)(texelPtr4 + texturePitch),
                *(uint32_t*)(texelPtr5 + texturePitch),
                *(uint32_t*)(texelPtr6 + texturePitch),
                *(uint32_t*)(texelPtr7 + texturePitch));

            __m256i sampleD = _mm256_setr_epi32(
                *(uint32_t*)(texelPtr0 + texturePitch + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr1 + texturePitch + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr2 + texturePitch + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr3 + texturePitch + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr4 + texturePitch + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr5 + texturePitch + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr6 + texturePitch + sizeof(uint32_t)),
                *(uint32_t*)(texelPtr7 + texturePitch + sizeof(uint32_t)));
#endif

            //END_TIMED_BLOCK(DrawBitmapFetch);

            //BEGIN_TIMED_BLOCK(DrawBitmapCompute);

            //compute coefficient for bilinear blend
            __m256 fX = _mm256_sub_ps(textureX, _mm256_cvtepi32_ps(fetchX));
            __m256 fY = _mm256_sub_ps(textureY, _mm256_cvtepi32_ps(fetchY));

            __m256 ifX = _mm256_sub_ps(oneReal, fX);
            __m256 ifY = _mm256_sub_ps(oneReal, fY);

            __m256 coef0 = _mm256_mul_ps(ifY, ifX);
            __m256 coef1 = _mm256_mul_ps(ifY, fX);
            __m256 coef2 = _mm256_mul_ps(fY, ifX);
            __m256 coef3 = _mm256_mul_ps(fY, fX);

            //0-255 srgb
            __m256 sampleAr = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleA, 16), pixelMask));
            __m256 sampleAg = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleA, 8), pixelMask));
            __m256 sampleAb = _mm256_cvtepi32_ps(_mm256_and_si256(sampleA, pixelMask));
            __m256 sampleAa = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleA, 24), pixelMask));

            __m256 sampleBr = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleB, 16), pixelMask));
            __m256 sampleBg = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleB, 8), pixelMask));
            __m256 sampleBb = _mm256_cvtepi32_ps(_mm256_and_si256(sampleB, pixelMask));
            __m256 sampleBa = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleB, 24), pixelMask));

            __m256 sampleCr = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleC, 16), pixelMask));
            __m256 sampleCg = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleC, 8), pixelMask));
            __m256 sampleCb = _mm256_cvtepi32_ps(_mm256_and_si256(sampleC, pixelMask));
            __m256 sampleCa = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleC, 24), pixelMask));

            __m256 sampleDr = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleD, 16), pixelMask));
            __m256 sampleDg = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleD, 8), pixelMask));
            __m256 sampleDb = _mm256_cvtepi32_ps(_mm256_and_si256(sampleD, pixelMask));
            __m256 sampleDa = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(sampleD, 24), pixelMask));

            __m256 sourceR = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(source, 16), pixelMask));
            __m256 sourceG = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(source, 8), pixelMask));
            __m256 sourceB = _mm256_cvtepi32_ps(_mm256_and_si256(source, pixelMask));
            __m256 sourceA = _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_srli_epi32(source, 24), pixelMask));
            
            //0-1 linear, probably computations can be done in 0-255*255 range rather than 0-1
            sampleAr = _mm256_mul_ps(sampleAr, inv255_8x);  
            sampleAr = _mm256_mul_ps(sampleAr, sampleAr);
            sampleAg = _mm256_mul_ps(sampleAg, inv255_8x);
            sampleAg = _mm256_mul_ps(sampleAg, sampleAg);
            sampleAb = _mm256_mul_ps(sampleAb, inv255_8x);
            sampleAb = _mm256_mul_ps(sampleAb, sampleAb);
            sampleAa = _mm256_mul_ps(sampleAa, inv255_8x);

            sampleBr = _mm256_mul_ps(sampleBr, inv255_8x);
            sampleBr = _mm256_mul_ps(sampleBr, sampleBr);
            sampleBg = _mm256_mul_ps(sampleBg, inv255_8x);
            sampleBg = _mm256_mul_ps(sampleBg, sampleBg);
            sampleBb = _mm256_mul_ps(sampleBb, inv255_8x);
            sampleBb = _mm256_mul_ps(sampleBb, sampleBb);
            sampleBa = _mm256_mul_ps(sampleBa, inv255_8x);

            sampleCr = _mm256_mul_ps(sampleCr, inv255_8x);
            sampleCr = _mm256_mul_ps(sampleCr, sampleCr);
            sampleCg = _mm256_mul_ps(sampleCg, inv255_8x);
            sampleCg = _mm256_mul_ps(sampleCg, sampleCg);
            sampleCb = _mm256_mul_ps(sampleCb, inv255_8x);
            sampleCb = _mm256_mul_ps(sampleCb, sampleCb);
            sampleCa = _mm256_mul_ps(sampleCa, inv255_8x);

            sampleDr = _mm256_mul_ps(sampleDr, inv255_8x);
            sampleDr = _mm256_mul_ps(sampleDr, sampleDr);
            sampleDg = _mm256_mul_ps(sampleDg, inv255_8x);
            sampleDg = _mm256_mul_ps(sampleDg, sampleDg);
            sampleDb = _mm256_mul_ps(sampleDb, inv255_8x);
            sampleDb = _mm256_mul_ps(sampleDb, sampleDb);
            sampleDa = _mm256_mul_ps(sampleDa, inv255_8x);

            sourceR = _mm256_mul_ps(sourceR, inv255_8x);
            sourceR = _mm256_mul_ps(sourceR, sourceR);
            sourceG = _mm256_mul_ps(sourceG, inv255_8x);
            sourceG = _mm256_mul_ps(sourceG, sourceG);
            sourceB = _mm256_mul_ps(sourceB, inv255_8x);
            sourceB = _mm256_mul_ps(sourceB, sourceB);
            sourceA = _mm256_mul_ps(sourceA, inv255_8x);


            //bilinear blend
            __m256 texelR = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(coef0, sampleAr), _mm256_mul_ps(coef1, sampleBr)),
                            _mm256_add_ps(_mm256_mul_ps(coef2, sampleCr), _mm256_mul_ps(coef3, sampleDr)));
            __m256 texelG = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(coef0, sampleAg), _mm256_mul_ps(coef1, sampleBg)),
                            _mm256_add_ps(_mm256_mul_ps(coef2, sampleCg), _mm256_mul_ps(coef3, sampleDg)));
            __m256 texelB = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(coef0, sampleAb), _mm256_mul_ps(coef1, sampleBb)),
                            _mm256_add_ps(_mm256_mul_ps(coef2, sampleCb), _mm256_mul_ps(coef3, sampleDb)));
            __m256 texelA = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(coef0, sampleAa), _mm256_mul_ps(coef1, sampleBa)),
                            _mm256_add_ps(_mm256_mul_ps(coef2, sampleCa), _mm256_mul_ps(coef3, sampleDa)));

            //blend with source
            __m256 invAlpha = _mm256_sub_ps(oneReal, texelA);
            __m256 resultingColorR = _mm256_add_ps(_mm256_mul_ps(invAlpha, sourceR), texelR);
            __m256 resultingColorG = _mm256_add_ps(_mm256_mul_ps(invAlpha, sourceG), texelG);
            __m256 resultingColorB = _mm256_add_ps(_mm256_mul_ps(invAlpha, sourceB), texelB);
            __m256 resultingColorA = _mm256_add_ps(_mm256_mul_ps(invAlpha, sourceA), texelA);

            //linear0-1 to sRGB0-255
            resultingColorR = _mm256_mul_ps(_mm256_mul_ps(_mm256_rsqrt_ps(resultingColorR), resultingColorR), one255_8x);
            resultingColorG = _mm256_mul_ps(_mm256_mul_ps(_mm256_rsqrt_ps(resultingColorG), resultingColorG), one255_8x);
            resultingColorB = _mm256_mul_ps(_mm256_mul_ps(_mm256_rsqrt_ps(resultingColorB), resultingColorB), one255_8x);
            resultingColorA = _mm256_mul_ps(resultingColorA, one255_8x);

            //pack for store
            __m256i intR = _mm256_cvtps_epi32(resultingColorR);
            __m256i intG = _mm256_cvtps_epi32(resultingColorG);
            __m256i intB = _mm256_cvtps_epi32(resultingColorB);
            __m256i intA = _mm256_cvtps_epi32(resultingColorA);

            intR = _mm256_slli_epi32(intR, 16);
            intG = _mm256_slli_epi32(intG, 8);
            intB = intB;
            intA = _mm256_slli_epi32(intA, 24);

            __m256i out = _mm256_or_si256(_mm256_or_si256(intR, intG), _mm256_or_si256(intB, intA));
            out = _mm256_or_si256(_mm256_and_si256(writeMask, out), _mm256_andnot_si256(writeMask, source));
            _mm256_store_si256((__m256i*)pixel, out);

            pixel += 8;
            pixelPosX = _mm256_add_ps(pixelPosX, Octo8x);

           // END_TIMED_BLOCK(DrawBitmapCompute);
        }
        row += outBuffer->m_Pitch;
    }

    END_TIMED_BLOCK(DrawBitmap)
}
#elif 1 //only for comparing as it's Casey Muratori code
void DrawBitmap(EngineOffScreenBuffer* Buffer, LoadedBitmap* Texture, Vector2 Origin, Vector2 XAxis, Vector2 YAxis)
{
    BEGIN_TIMED_BLOCK(DrawBitmap);

    float XAxisLength = Length(XAxis);
    float YAxisLength = Length(YAxis);

    Vector2 NxAxis = (YAxisLength / XAxisLength) * XAxis;
    Vector2 NyAxis = (XAxisLength / YAxisLength) * YAxis;

    float InvXAxisLengthSq = 1.0f / LengthSq(XAxis);
    float InvYAxisLengthSq = 1.0f / LengthSq(YAxis);

    const int32_t widthMax = (int32_t)Buffer->m_Width - 1;
    const int32_t heightMax = (int32_t)Buffer->m_Height - 1;

    int32_t xMin = widthMax;
    int32_t xMax = 0;
    int32_t yMin = heightMax;
    int32_t yMax = 0;

    Vector2 P[4] = { Origin, Origin + XAxis, Origin + XAxis + YAxis, Origin + YAxis };
    for (int PIndex = 0;
        PIndex < 4;
        ++PIndex)
    {
        Vector2 TestP = P[PIndex];
        int FloorX = FloorFloatInt32(TestP.x);
        int CeilX = CeilFloatInt32(TestP.x) + 1;
        int FloorY = FloorFloatInt32(TestP.y);
        int CeilY = CeilFloatInt32(TestP.y) + 1;

        xMin = xMin > FloorX ? FloorX : xMin;
        yMin = yMin > FloorY ? FloorY : yMin;
        xMax = xMax < CeilX ? CeilX : xMax;
        yMax = yMax < CeilY ? CeilY : yMax;
    }

    xMin = xMin < 0 ? 0 : xMin;
    yMin = yMin < 0 ? 0 : yMin;
    xMax = xMax > widthMax ? widthMax : xMax;
    yMax = yMax > heightMax ? heightMax : yMax;


    if (xMin < xMax && yMin < yMax)
    {
        __m128i StartClipMask = _mm_set1_epi8(-1);
        __m128i EndClipMask = _mm_set1_epi8(-1);

        /*__m128i StartClipMasks[] =
        {
            _mm_slli_si128(StartClipMask, 0 * 4),
            _mm_slli_si128(StartClipMask, 1 * 4),
            _mm_slli_si128(StartClipMask, 2 * 4),
            _mm_slli_si128(StartClipMask, 3 * 4),
        };

        __m128i EndClipMasks[] =
        {
            _mm_srli_si128(EndClipMask, 0 * 4),
            _mm_srli_si128(EndClipMask, 3 * 4),
            _mm_srli_si128(EndClipMask, 2 * 4),
            _mm_srli_si128(EndClipMask, 1 * 4),
        };*/

        if (xMin & 3)
        {
            //StartClipMask = StartClipMasks[xMin & 3];
            xMin = xMin & ~3;
        }

        if (xMax & 3)
        {
            //EndClipMask = EndClipMasks[xMax & 3];
            xMax = (xMax & ~3) + 4;
        }

        Vector2 nXAxis = InvXAxisLengthSq * XAxis;
        Vector2 nYAxis = InvYAxisLengthSq * YAxis;

        float Inv255 = 1.0f / 255.0f;
        __m128 Inv255_4x = _mm_set1_ps(Inv255);
        float One255 = 255.0f;

        __m128 One = _mm_set1_ps(1.0f);
        __m128 Half = _mm_set1_ps(0.5f);
        __m128 Four_4x = _mm_set1_ps(4.0f);
        __m128 One255_4x = _mm_set1_ps(255.0f);
        __m128 Zero = _mm_set1_ps(0.0f);
        __m128i MaskFF = _mm_set1_epi32(0xFF);
        __m128i MaskFFFF = _mm_set1_epi32(0xFFFF);
        __m128i MaskFF00FF = _mm_set1_epi32(0x00FF00FF);
        __m128 nXAxisx_4x = _mm_set1_ps(nXAxis.x);
        __m128 nXAxisy_4x = _mm_set1_ps(nXAxis.y);
        __m128 nYAxisx_4x = _mm_set1_ps(nYAxis.x);
        __m128 nYAxisy_4x = _mm_set1_ps(nYAxis.y);
        __m128 Originx_4x = _mm_set1_ps(Origin.x);
        __m128 Originy_4x = _mm_set1_ps(Origin.y);
        __m128 MaxColorValue = _mm_set1_ps(255.0f * 255.0f);
        __m128i TexturePitch_4x = _mm_set1_epi32(Texture->Pitch);

        __m128 WidthM2 = _mm_set1_ps((float)(Texture->Width - 2));
        __m128 HeightM2 = _mm_set1_ps((float)(Texture->Height - 2));

        uint8_t* Row = ((uint8_t*)Buffer->m_Memory +
            xMin * 4 +
            yMin * Buffer->m_Pitch);

        void* TextureMemory = Texture->Memory;
        int32_t TexturePitch = Texture->Pitch;

        for (int Y = yMin;
            Y < yMax;
            Y += 1)
        {
            __m128 PixelPy = _mm_set1_ps((float)Y);
            PixelPy = _mm_sub_ps(PixelPy, Originy_4x);
            __m128 PynX = _mm_mul_ps(PixelPy, nXAxisy_4x);
            __m128 PynY = _mm_mul_ps(PixelPy, nYAxisy_4x);

            __m128 PixelPx = _mm_set_ps((float)(xMin + 3),
                (float)(xMin + 2),
                (float)(xMin + 1),
                (float)(xMin + 0));
            PixelPx = _mm_sub_ps(PixelPx, Originx_4x);

            //__m128i ClipMask = StartClipMask;

            uint32_t* Pixel = (uint32_t*)Row;
            for (int XI = xMin;
                XI < xMax;
                XI += 4)
            {
#define mmSquare(a) _mm_mul_ps(a, a)    
#define M(a, i) ((float *)&(a))[i]
#define Mi(a, i) ((uint32_t *)&(a))[i]

                __m128 U = _mm_add_ps(_mm_mul_ps(PixelPx, nXAxisx_4x), PynX);
                __m128 V = _mm_add_ps(_mm_mul_ps(PixelPx, nYAxisx_4x), PynY);

                __m128i WriteMask = _mm_castps_si128(_mm_and_ps(_mm_and_ps(_mm_cmpge_ps(U, Zero),
                    _mm_cmple_ps(U, One)),
                    _mm_and_ps(_mm_cmpge_ps(V, Zero),
                        _mm_cmple_ps(V, One))));
                //WriteMask = _mm_and_si128(WriteMask, ClipMask);

                // TODO(casey): Later, re-check if this helps
                //            if(_mm_movemask_epi8(WriteMask))
                {
                    __m128i OriginalDest = _mm_load_si128((__m128i*)Pixel);

                    U = _mm_min_ps(_mm_max_ps(U, Zero), One);
                    V = _mm_min_ps(_mm_max_ps(V, Zero), One);

                    // NOTE(casey): Bias texture coordinates to start
                    // on the boundary between the 0,0 and 1,1 pixels.
                    __m128 tX = _mm_add_ps(_mm_mul_ps(U, WidthM2), Half);
                    __m128 tY = _mm_add_ps(_mm_mul_ps(V, HeightM2), Half);

                    __m128i FetchX_4x = _mm_cvttps_epi32(tX);
                    __m128i FetchY_4x = _mm_cvttps_epi32(tY);

                    __m128 fX = _mm_sub_ps(tX, _mm_cvtepi32_ps(FetchX_4x));
                    __m128 fY = _mm_sub_ps(tY, _mm_cvtepi32_ps(FetchY_4x));

                    FetchX_4x = _mm_slli_epi32(FetchX_4x, 2);
                    FetchY_4x = _mm_or_si128(_mm_mullo_epi16(FetchY_4x, TexturePitch_4x),
                        _mm_slli_epi32(_mm_mulhi_epi16(FetchY_4x, TexturePitch_4x), 16));
                    __m128i Fetch_4x = _mm_add_epi32(FetchX_4x, FetchY_4x);


                    //BEGIN_TIMED_BLOCK(DrawBitmapFetch);

                    int32_t Fetch0 = Mi(Fetch_4x, 0);
                    int32_t Fetch1 = Mi(Fetch_4x, 1);
                    int32_t Fetch2 = Mi(Fetch_4x, 2);
                    int32_t Fetch3 = Mi(Fetch_4x, 3);


                    uint8_t* TexelPtr0 = ((uint8_t*)TextureMemory) + Fetch0;
                    uint8_t* TexelPtr1 = ((uint8_t*)TextureMemory) + Fetch1;
                    uint8_t* TexelPtr2 = ((uint8_t*)TextureMemory) + Fetch2;
                    uint8_t* TexelPtr3 = ((uint8_t*)TextureMemory) + Fetch3;

                    __m128i SampleA = _mm_setr_epi32(*(uint32_t*)(TexelPtr0),
                        *(uint32_t*)(TexelPtr1),
                        *(uint32_t*)(TexelPtr2),
                        *(uint32_t*)(TexelPtr3));

                    __m128i SampleB = _mm_setr_epi32(*(uint32_t*)(TexelPtr0 + sizeof(uint32_t)),
                        *(uint32_t*)(TexelPtr1 + sizeof(uint32_t)),
                        *(uint32_t*)(TexelPtr2 + sizeof(uint32_t)),
                        *(uint32_t*)(TexelPtr3 + sizeof(uint32_t)));

                    __m128i SampleC = _mm_setr_epi32(*(uint32_t*)(TexelPtr0 + TexturePitch),
                        *(uint32_t*)(TexelPtr1 + TexturePitch),
                        *(uint32_t*)(TexelPtr2 + TexturePitch),
                        *(uint32_t*)(TexelPtr3 + TexturePitch));

                    __m128i SampleD = _mm_setr_epi32(*(uint32_t*)(TexelPtr0 + TexturePitch + sizeof(uint32_t)),
                        *(uint32_t*)(TexelPtr1 + TexturePitch + sizeof(uint32_t)),
                        *(uint32_t*)(TexelPtr2 + TexturePitch + sizeof(uint32_t)),
                        *(uint32_t*)(TexelPtr3 + TexturePitch + sizeof(uint32_t)));

                    //END_TIMED_BLOCK(DrawBitmapFetch);

                    //BEGIN_TIMED_BLOCK(DrawBitmapCompute);

                    // NOTE(casey): Unpack bilinear samples
                    __m128i TexelArb = _mm_and_si128(SampleA, MaskFF00FF);
                    __m128i TexelAag = _mm_and_si128(_mm_srli_epi32(SampleA, 8), MaskFF00FF);
                    TexelArb = _mm_mullo_epi16(TexelArb, TexelArb);
                    __m128 TexelAa = _mm_cvtepi32_ps(_mm_srli_epi32(TexelAag, 16));
                    TexelAag = _mm_mullo_epi16(TexelAag, TexelAag);

                    __m128i TexelBrb = _mm_and_si128(SampleB, MaskFF00FF);
                    __m128i TexelBag = _mm_and_si128(_mm_srli_epi32(SampleB, 8), MaskFF00FF);
                    TexelBrb = _mm_mullo_epi16(TexelBrb, TexelBrb);
                    __m128 TexelBa = _mm_cvtepi32_ps(_mm_srli_epi32(TexelBag, 16));
                    TexelBag = _mm_mullo_epi16(TexelBag, TexelBag);

                    __m128i TexelCrb = _mm_and_si128(SampleC, MaskFF00FF);
                    __m128i TexelCag = _mm_and_si128(_mm_srli_epi32(SampleC, 8), MaskFF00FF);
                    TexelCrb = _mm_mullo_epi16(TexelCrb, TexelCrb);
                    __m128 TexelCa = _mm_cvtepi32_ps(_mm_srli_epi32(TexelCag, 16));
                    TexelCag = _mm_mullo_epi16(TexelCag, TexelCag);

                    __m128i TexelDrb = _mm_and_si128(SampleD, MaskFF00FF);
                    __m128i TexelDag = _mm_and_si128(_mm_srli_epi32(SampleD, 8), MaskFF00FF);
                    TexelDrb = _mm_mullo_epi16(TexelDrb, TexelDrb);
                    __m128 TexelDa = _mm_cvtepi32_ps(_mm_srli_epi32(TexelDag, 16));
                    TexelDag = _mm_mullo_epi16(TexelDag, TexelDag);

                    // NOTE(casey): Load destination
                    __m128 Destb = _mm_cvtepi32_ps(_mm_and_si128(OriginalDest, MaskFF));
                    __m128 Destg = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(OriginalDest, 8), MaskFF));
                    __m128 Destr = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(OriginalDest, 16), MaskFF));
                    __m128 Desta = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(OriginalDest, 24), MaskFF));

                    // NOTE(casey): Convert texture from 0-255 sRGB to "linear" 0-1 brightness space
                    __m128 TexelAr = _mm_cvtepi32_ps(_mm_srli_epi32(TexelArb, 16));
                    __m128 TexelAg = _mm_cvtepi32_ps(_mm_and_si128(TexelAag, MaskFFFF));
                    __m128 TexelAb = _mm_cvtepi32_ps(_mm_and_si128(TexelArb, MaskFFFF));

                    __m128 TexelBr = _mm_cvtepi32_ps(_mm_srli_epi32(TexelBrb, 16));
                    __m128 TexelBg = _mm_cvtepi32_ps(_mm_and_si128(TexelBag, MaskFFFF));
                    __m128 TexelBb = _mm_cvtepi32_ps(_mm_and_si128(TexelBrb, MaskFFFF));

                    __m128 TexelCr = _mm_cvtepi32_ps(_mm_srli_epi32(TexelCrb, 16));
                    __m128 TexelCg = _mm_cvtepi32_ps(_mm_and_si128(TexelCag, MaskFFFF));
                    __m128 TexelCb = _mm_cvtepi32_ps(_mm_and_si128(TexelCrb, MaskFFFF));

                    __m128 TexelDr = _mm_cvtepi32_ps(_mm_srli_epi32(TexelDrb, 16));
                    __m128 TexelDg = _mm_cvtepi32_ps(_mm_and_si128(TexelDag, MaskFFFF));
                    __m128 TexelDb = _mm_cvtepi32_ps(_mm_and_si128(TexelDrb, MaskFFFF));

                    // NOTE(casey): Bilinear texture blend
                    __m128 ifX = _mm_sub_ps(One, fX);
                    __m128 ifY = _mm_sub_ps(One, fY);

                    __m128 l0 = _mm_mul_ps(ifY, ifX);
                    __m128 l1 = _mm_mul_ps(ifY, fX);
                    __m128 l2 = _mm_mul_ps(fY, ifX);
                    __m128 l3 = _mm_mul_ps(fY, fX);

                    __m128 Texelr = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, TexelAr), _mm_mul_ps(l1, TexelBr)),
                        _mm_add_ps(_mm_mul_ps(l2, TexelCr), _mm_mul_ps(l3, TexelDr)));
                    __m128 Texelg = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, TexelAg), _mm_mul_ps(l1, TexelBg)),
                        _mm_add_ps(_mm_mul_ps(l2, TexelCg), _mm_mul_ps(l3, TexelDg)));
                    __m128 Texelb = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, TexelAb), _mm_mul_ps(l1, TexelBb)),
                        _mm_add_ps(_mm_mul_ps(l2, TexelCb), _mm_mul_ps(l3, TexelDb)));
                    __m128 Texela = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, TexelAa), _mm_mul_ps(l1, TexelBa)),
                        _mm_add_ps(_mm_mul_ps(l2, TexelCa), _mm_mul_ps(l3, TexelDa)));

                    Texelr = _mm_min_ps(_mm_max_ps(Texelr, Zero), MaxColorValue);
                    Texelg = _mm_min_ps(_mm_max_ps(Texelg, Zero), MaxColorValue);
                    Texelb = _mm_min_ps(_mm_max_ps(Texelb, Zero), MaxColorValue);

                    // NOTE(casey): Go from sRGB to "linear" brightness space
                    Destr = mmSquare(Destr);
                    Destg = mmSquare(Destg);
                    Destb = mmSquare(Destb);

                    // NOTE(casey): Destination blend
                    __m128 InvTexelA = _mm_sub_ps(One, _mm_mul_ps(Inv255_4x, Texela));
                    __m128 Blendedr = _mm_add_ps(_mm_mul_ps(InvTexelA, Destr), Texelr);
                    __m128 Blendedg = _mm_add_ps(_mm_mul_ps(InvTexelA, Destg), Texelg);
                    __m128 Blendedb = _mm_add_ps(_mm_mul_ps(InvTexelA, Destb), Texelb);
                    __m128 Blendeda = _mm_add_ps(_mm_mul_ps(InvTexelA, Desta), Texela);

                    // NOTE(casey): Go from "linear" 0-1 brightness space to sRGB 0-255
#if 1
                    Blendedr = _mm_mul_ps(Blendedr, _mm_rsqrt_ps(Blendedr));
                    Blendedg = _mm_mul_ps(Blendedg, _mm_rsqrt_ps(Blendedg));
                    Blendedb = _mm_mul_ps(Blendedb, _mm_rsqrt_ps(Blendedb));
#else
                    Blendedr = _mm_sqrt_ps(Blendedr);
                    Blendedg = _mm_sqrt_ps(Blendedg);
                    Blendedb = _mm_sqrt_ps(Blendedb);
#endif
                    Blendeda = Blendeda;

                    __m128i Intr = _mm_cvtps_epi32(Blendedr);
                    __m128i Intg = _mm_cvtps_epi32(Blendedg);
                    __m128i Intb = _mm_cvtps_epi32(Blendedb);
                    __m128i Inta = _mm_cvtps_epi32(Blendeda);

                    __m128i Sr = _mm_slli_epi32(Intr, 16);
                    __m128i Sg = _mm_slli_epi32(Intg, 8);
                    __m128i Sb = Intb;
                    __m128i Sa = _mm_slli_epi32(Inta, 24);

                    __m128i Out = _mm_or_si128(_mm_or_si128(Sr, Sg), _mm_or_si128(Sb, Sa));

                    __m128i MaskedOut = _mm_or_si128(_mm_and_si128(WriteMask, Out),
                        _mm_andnot_si128(WriteMask, OriginalDest));
                    _mm_store_si128((__m128i*)Pixel, MaskedOut);

                    //END_TIMED_BLOCK(DrawBitmapCompute);
                }

                PixelPx = _mm_add_ps(PixelPx, Four_4x);
                Pixel += 4;

               /* if ((XI + 8) < xMax)
                {
                    ClipMask = _mm_set1_epi8(-1);
                }
                else
                {
                    ClipMask = EndClipMask;
                }*/
            }

            Row += Buffer->m_Pitch;
        }
    }

    END_TIMED_BLOCK(DrawBitmap);
}
#endif