#pragma once

#include <cstdint>

#define KILOBYTES(Value) ((Value) * 1024)
#define MEGABYTES(Value) (KILOBYTES(Value) * 1024)
#define GIGABYTES(Value) (MEGABYTES(Value) * 1024)
#define TERABYTES(Value) (GIGABYTES(Value) * 1024)

constexpr int MaxGamePadControllersNum = 4;

struct EngineOffScreenBuffer
{
    void* m_Memory = nullptr;
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_BytesPerPixel = 0;
};

struct ButtonState
{
    uint8_t TransitionsCount = 0;
    bool EndedDown = false;
};

struct GameControllerInput
{
    float m_MoveAxisX = 0.0f;
    float m_MoveAxisY = 0.0f;

    float m_TurnAxisX = 0.0f;
    float m_TurnAxisY = 0.0f;

    ButtonState m_Up;
    ButtonState m_Down;
    ButtonState m_Left;
    ButtonState m_Right;
    ButtonState m_LeftShoulder;
    ButtonState m_RightShoulder;

};

struct GameInput
{
    GameControllerInput m_KeyboardMouseController;
    GameControllerInput m_ControllersInput[MaxGamePadControllersNum];
};

struct GameMemory
{
    uint64_t m_PersistentMemorySize;
    void* m_PersistentStorage;
    uint64_t m_TransientMemorySize;
    void* m_TransientStorage;
};

struct GameState
{
    float m_xOffset;
    float m_yOffset;
};

typedef void InitializeSignature(GameMemory* gameMemory);
extern "C"  void Initialize(GameMemory* gameMemory);

typedef void UpdateAndRenderSignature(GameMemory* gameMemory, GameInput* gameInput, EngineOffScreenBuffer* outBuffer);
extern "C" void UpdateAndRender(GameMemory* gameMemory, GameInput * gameInput, EngineOffScreenBuffer* outBuffer);