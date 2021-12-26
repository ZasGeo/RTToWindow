// RTToWindow.cpp : Defines the entry point for the application.
//

#include <Windows.h>
#include <windowsx.h>
#include <Xinput.h>
#include <cstdint>
#include <stdio.h>

#include <Win32Layer/Win32Layer.h>
#include <Engine/Engine_platform.h>

static bool g_Running;
static Win32OffScreenBuffer g_BackBuffer;
static WINDOWPLACEMENT g_WindowPosition = { sizeof(g_WindowPosition) };
static LARGE_INTEGER g_PerfCountFrequency;

void Win32ToggleFullscreen(HWND window)
{
    // http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx

    DWORD style = GetWindowLong(window, GWL_STYLE);
    if (style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        if (GetWindowPlacement(window, &g_WindowPosition) &&
            GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
        {
            SetWindowLong(window, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP,
                monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(window, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &g_WindowPosition);
        SetWindowPos(window, 0, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

Win32WindowDimension Win32GetWinDimension(HWND window)
{
    Win32WindowDimension result;

    RECT clientRect;
    GetClientRect(window, &clientRect);
    result.m_Width = clientRect.right - clientRect.left;
    result.m_Height = clientRect.bottom - clientRect.top;
    return result;
}

void Win32ResizeOffScreenBuffer(Win32OffScreenBuffer* buffer, const int width, const int height)
{
    if (buffer->m_Memory != NULL)
    {
        VirtualFree(buffer->m_Memory, 0, MEM_RELEASE);
    }

    buffer->m_Width = width;
    buffer->m_Height = height;
    buffer->m_Pitch = width * BUFFER_BYTES_PER_PIXEL;
    buffer->m_AspectRatio = static_cast<float>(width) / height;

    buffer->m_Info.bmiHeader.biSize = sizeof(buffer->m_Info.bmiHeader);
    buffer->m_Info.bmiHeader.biWidth = width;
    buffer->m_Info.bmiHeader.biHeight = height;
    buffer->m_Info.bmiHeader.biPlanes = 1;
    buffer->m_Info.bmiHeader.biBitCount = 32;
    buffer->m_Info.bmiHeader.biCompression = BI_RGB;

    const int bitMapSize = buffer->m_Width * buffer->m_Height * BUFFER_BYTES_PER_PIXEL;
    buffer->m_Memory = VirtualAlloc(0, bitMapSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void Win32PresentBufferToWindow(Win32OffScreenBuffer buffer, HDC deviceContext, uint32_t width, uint32_t height)
{
    const float windowAspect = static_cast<float>(width) / height;

    const float heightAspected = buffer.m_AspectRatio * height;

    uint32_t newWidth = width;
    uint32_t newHeight = height;
    uint32_t offsetX = 0;
    uint32_t offsetY = 0;
    if (windowAspect >= buffer.m_AspectRatio)
    {
        const float widthMult = heightAspected / width;
        newWidth = static_cast<uint32_t>(width * widthMult);
        offsetX = (width - newWidth) / 2;
        
        PatBlt(deviceContext, 0, 0, offsetX, height, BLACKNESS);
        PatBlt(deviceContext, width - offsetX, 0, offsetX, height, BLACKNESS);
    }
    else
    {
        const float heightMult = width / heightAspected;
        newHeight = static_cast<uint32_t>(height * heightMult);
        offsetY = (height - newHeight) / 2;

        PatBlt(deviceContext, 0, 0, width, offsetY, BLACKNESS);
        PatBlt(deviceContext, 0, height - offsetY, width, offsetY, BLACKNESS);
    }

    StretchDIBits(
        deviceContext,
        offsetX, offsetY, newWidth, newHeight,
        0, 0, buffer.m_Width, buffer.m_Height,
        buffer.m_Memory,
        &buffer.m_Info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

void Win32ProccessXInputButton(DWORD xInputButtonState, ButtonState* oldState, DWORD buttonBit, ButtonState* outNewState)
{
    outNewState->EndedDown = (xInputButtonState & buttonBit) == buttonBit;
    outNewState->TransitionsCount = oldState->EndedDown != outNewState->EndedDown ? 1 : 0;
}

float Win32GetXInputStickValue(SHORT inValue, SHORT deadZone)
{
    const SHORT valueDeadZoned = static_cast<int>(inValue > deadZone || inValue < -deadZone) * inValue;

    return ((static_cast<float>(valueDeadZoned) + 32768.0f) / 65536.0f) * 2.0f - 1.0f;
}

void UpdateAndRenderStub(float dt, GameMemory* gameMemory, GameInput* gameInput, EngineOffScreenBuffer* outBuffer)
{
}

LARGE_INTEGER Win32GetWallClock()
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

float Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    float result = static_cast<float>(End.QuadPart - Start.QuadPart) / g_PerfCountFrequency.QuadPart;
    return result;
}

FILETIME Win32GetLastWriteTime(const char* filename)
{
    FILETIME result = {};
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesExA(filename, GetFileExInfoStandard, &data))
    {
        result = data.ftLastWriteTime;
    }
    return result;
}

Win32GameCode Win32LoadGameCode()
{
    Win32GameCode result = {};   
    result.UpdateAndRender = UpdateAndRenderStub;

    if (CopyFileA("Engine.dll", "Engine_tmp.dll", FALSE))
    {
        result.m_Module = LoadLibraryA("Engine_tmp.dll");
        if (result.m_Module != NULL)
        {
            result.m_LastWriteTime = Win32GetLastWriteTime("Engine.dll");
            result.UpdateAndRender = ((UpdateAndRenderSignature*)GetProcAddress(result.m_Module, "UpdateAndRender"));
            result.Initialize = ((InitializeSignature*)GetProcAddress(result.m_Module, "Initialize"));
        }
    }

    return result;
}

void Win32UnloadGameCode(Win32GameCode* gameCode)
{
    if (gameCode->m_Module != NULL)
    {
        FreeLibrary(gameCode->m_Module);
        gameCode->m_Module = NULL;
    }
    gameCode->UpdateAndRender = UpdateAndRenderStub;
    gameCode->Initialize = nullptr;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = {};
    switch (uMsg)
    {
    case WM_SIZE:
    {
        Win32WindowDimension dimension = Win32GetWinDimension(hWnd);
        Win32ResizeOffScreenBuffer(&g_BackBuffer, 960, 540);
    }
    break;
    case WM_DESTROY:
    case WM_CLOSE:
        g_Running = false;
        break;
    case WM_ACTIVATEAPP:
        break;
    default:
        result = DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wndClass = {};
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClass.lpfnWndProc = &WindowProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = L"RT Test";

    if (!RegisterClass(&wndClass))
    {
        return EXIT_FAILURE;
    }

    HWND windowHandle = CreateWindowEx
    (
        0,
        wndClass.lpszClassName,
        L"RT Test Window",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        0
    );

    if (windowHandle == NULL)
    {
        return EXIT_FAILURE;
    }

    HDC deviceContext = GetDC(windowHandle);
    if (deviceContext == NULL)
    {
        return EXIT_FAILURE;
    }

    Win32GameCode gameCode = Win32LoadGameCode();
    if (gameCode.m_Module == NULL)
    {
        return EXIT_FAILURE;
    }
    
#ifdef INTERNAL_USAGE_BUILD
    LPVOID gameMemoryStartAdress = (LPVOID)TERABYTES(4ull);   
#else
    LPVOID gameMemoryStartAdress = NULL;
#endif // INTERNAL_USAGE

    SYSTEM_INFO inf = {};
    GetSystemInfo(&inf);

    GameMemory gameMemory = {};
    gameMemory.m_PersistentMemorySize = MEGABYTES(64ull);
    gameMemory.m_TransientMemorySize = GIGABYTES(1ull);
    gameMemory.m_PersistentStorage = VirtualAlloc(gameMemoryStartAdress, gameMemory.m_PersistentMemorySize + gameMemory.m_TransientMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    gameMemory.m_TransientStorage = static_cast<char*>(gameMemory.m_PersistentStorage) + gameMemory.m_PersistentMemorySize;

    if (gameMemory.m_PersistentStorage == NULL)
    {
        return EXIT_FAILURE;
    }

    POINT prevCursorPos = {};
    if (GetCursorPos(&prevCursorPos))
    {
        if (!ScreenToClient(windowHandle, &prevCursorPos))
        {
            return EXIT_FAILURE;
        }
    }

    QueryPerformanceFrequency(&g_PerfCountFrequency);

    LARGE_INTEGER lastCounter = Win32GetWallClock();

    GameInput input[2] = {};
    GameInput* newInput = &input[0];
    GameInput* oldInput = &input[1];

    gameCode.Initialize(&gameMemory);

    uint64_t lastCycleCount = __rdtsc();

    float dt = 0.0f;
    g_Running = true;
    while (g_Running)
    {
        FILETIME engineDLLWriteTime = Win32GetLastWriteTime("Engine.dll");
        if(CompareFileTime(&gameCode.m_LastWriteTime, &engineDLLWriteTime) == -1)
        {
            Win32UnloadGameCode(&gameCode);
            gameCode = Win32LoadGameCode();                      
        }

        newInput->m_KeyboardMouseController = oldInput->m_KeyboardMouseController;

        MSG message;
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            switch (message.message)
            {
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            {
                const bool altKeyWasDown = (message.lParam & (1 << 29)) != 0;
                const bool wasDown = (message.lParam & (static_cast<LPARAM>(1) << 30)) != 0;
                const bool isDown = (message.lParam & (static_cast<LPARAM>(1) << 31)) == 0;
                if (isDown != wasDown)
                {
                    switch (message.wParam)
                    {
                    case 'W':
                        newInput->m_KeyboardMouseController.m_MoveAxisY += isDown ? 1.0f : -1.0f;
                        break;
                    case 'S':
                        newInput->m_KeyboardMouseController.m_MoveAxisY += isDown ? -1.0f : 1.0f;
                        break;
                    case 'D':
                        newInput->m_KeyboardMouseController.m_MoveAxisX += isDown ? 1.0f : -1.0f;
                        break;
                    case 'A':
                        newInput->m_KeyboardMouseController.m_MoveAxisX += isDown ? -1.0f : 1.0f;
                        break;
                    case VK_RETURN:
                        if (isDown && altKeyWasDown)
                        {
                            Win32ToggleFullscreen(windowHandle);
                        }
                        break;
                    case VK_F4:
                        if (isDown && altKeyWasDown)
                        {
                            g_Running = false;
                        };
                    default:
                        break;
                    }
                }
            }break;
            case WM_MOUSEMOVE: //super base mouse handling, no account for well anything
            {
                const LONG xPos = GET_X_LPARAM(message.lParam);
                const LONG yPos = GET_Y_LPARAM(message.lParam);
              
                newInput->m_KeyboardMouseController.m_TurnAxisX = static_cast<float>(xPos - prevCursorPos.x) / 100.0f;
                newInput->m_KeyboardMouseController.m_TurnAxisY = static_cast<float>(yPos - prevCursorPos.y) / 100.0f;

                prevCursorPos.x = xPos;
                prevCursorPos.y = yPos;

            }break;
            default:
                TranslateMessage(&message);
                DispatchMessage(&message);
                break;
            }
        }

        const int MaxControllers = XUSER_MAX_COUNT > MaxGamePadControllersNum ? MaxGamePadControllersNum : XUSER_MAX_COUNT;

        for (DWORD controllerIndex = 0; controllerIndex < MaxControllers; ++controllerIndex)
        {
            GameControllerInput* oldController = &oldInput->m_ControllersInput[controllerIndex];
            GameControllerInput* newController = &newInput->m_ControllersInput[controllerIndex];

            XINPUT_STATE controllerState;
            if (XInputGetState(controllerIndex, &controllerState) == ERROR_SUCCESS)
            {
                Win32ProccessXInputButton(controllerState.Gamepad.wButtons, &oldController->m_Up, XINPUT_GAMEPAD_Y, &newController->m_Up);
                Win32ProccessXInputButton(controllerState.Gamepad.wButtons, &oldController->m_Down, XINPUT_GAMEPAD_A, &newController->m_Down);
                Win32ProccessXInputButton(controllerState.Gamepad.wButtons, &oldController->m_Left, XINPUT_GAMEPAD_X, &newController->m_Left);
                Win32ProccessXInputButton(controllerState.Gamepad.wButtons, &oldController->m_Right, XINPUT_GAMEPAD_B, &newController->m_Right);
                Win32ProccessXInputButton(controllerState.Gamepad.wButtons, &oldController->m_LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER, &newController->m_LeftShoulder);
                Win32ProccessXInputButton(controllerState.Gamepad.wButtons, &oldController->m_RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER, &newController->m_RightShoulder);

                newController->m_MoveAxisX = Win32GetXInputStickValue(controllerState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                newController->m_MoveAxisY = Win32GetXInputStickValue(controllerState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

                newController->m_TurnAxisX = Win32GetXInputStickValue(controllerState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
                newController->m_TurnAxisY = Win32GetXInputStickValue(controllerState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
            }
        }

        EngineOffScreenBuffer offscreenBuffer;
        offscreenBuffer.m_Memory = g_BackBuffer.m_Memory;
        offscreenBuffer.m_Width = g_BackBuffer.m_Width;
        offscreenBuffer.m_Height = g_BackBuffer.m_Height;
        offscreenBuffer.m_Pitch = g_BackBuffer.m_Pitch;
        offscreenBuffer.m_AspectRatio = g_BackBuffer.m_AspectRatio;
        gameCode.UpdateAndRender(dt, &gameMemory, newInput, &offscreenBuffer);

        Win32WindowDimension dimension =  Win32GetWinDimension(windowHandle);     
        Win32PresentBufferToWindow(g_BackBuffer, deviceContext, dimension.m_Width, dimension.m_Height);

        GameInput* tmpInput = oldInput;
        oldInput = newInput;
        newInput = tmpInput;

        LARGE_INTEGER endCounter = Win32GetWallClock();;
        dt = Win32GetSecondsElapsed(lastCounter, endCounter);
        lastCounter = endCounter;
 
        uint64_t endCycleCount = __rdtsc();
        uint64_t cyclesElapsed = endCycleCount - lastCycleCount;
        lastCycleCount = endCycleCount;
 
        const double megaCyclesPerFrame = static_cast<double>(cyclesElapsed) / (1000.0 * 1000.0);
        const double msPerFrame = 1000.0 * dt;
        char fpsBuffer[256];
        _snprintf_s(fpsBuffer, sizeof(fpsBuffer), "%.03fms/f,  %.02fmc/f\n", msPerFrame, megaCyclesPerFrame);
        OutputDebugStringA(fpsBuffer);
    }

    //no, you don't need to clear any resource here
    return EXIT_SUCCESS;
}