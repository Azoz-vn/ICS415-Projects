#include "cross_platform_window.hpp"


CrossPlatformWindow::CrossPlatformWindow(int width, int height, const std::string &windowTitle) {
    if (width > 200) this->width = width;
    if (height > 200) this->height = height;
    this->title = windowTitle;

#ifdef _WIN32
    static constexpr auto className = "CrossPlatformWindowClass";
    static bool windowClassRegistered = false;

    HINSTANCE hinstance = GetModuleHandle(nullptr);

    if (!windowClassRegistered) {
        WNDCLASSEX windowClass = {};

        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = Win32WindowProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = sizeof(LONG_PTR);
        windowClass.hInstance = hinstance;
        windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windowClass.hbrBackground = nullptr;
        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = className;

        if (!RegisterClassEx(&windowClass)) {
            DWORD error = GetLastError();
            char message[512];
            snprintf(message, sizeof(message),
                     "Failed to register window class. Error code: %lu", error);
            MessageBoxA(nullptr, message, "Win32 Error", MB_OK | MB_ICONERROR);
            exit(-1);
        }

        windowClassRegistered = true;

    }

    constexpr DWORD dwordStyles = WS_OVERLAPPEDWINDOW;
    RECT adjustedRect = { 0, 0, width, height };
    AdjustWindowRect(&adjustedRect, dwordStyles, FALSE);

    hwnd = CreateWindowEx(
        0,
        className,                // static class name
        title.c_str(),            // window title (can be dynamic)
        dwordStyles,
        CW_USEDEFAULT, CW_USEDEFAULT,
        adjustedRect.right - adjustedRect.left,
        adjustedRect.bottom - adjustedRect.top,
        nullptr, nullptr,
        hinstance,
        nullptr
    );

    if (hwnd == nullptr) {
        DWORD error = GetLastError();
        char message[512];
        snprintf(message, sizeof(message),
                 "Failed to create Win32 window. Error code: %lu", error);
        MessageBoxA(nullptr, message, "Win32 Error", MB_OK | MB_ICONERROR);
        exit(-1);
    }

    ShowWindow(hwnd, SW_SHOW);
    deviceContext = GetDC(hwnd);
    openglContext = nullptr;

    memset(keybaord.keysPressed, 0, sizeof keybaord.keysPressed);
    memset(keybaord.keysToggled, 0, sizeof keybaord.keysToggled);

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
#endif

    this->isCurrentlyOpen = true;
    this->isKeyboardFocused = true;
}


CrossPlatformWindow::~CrossPlatformWindow() {

#ifdef _WIN32
    wglMakeCurrent(nullptr, nullptr);  // Detach the OpenGL context from the current thread.

    if (openglContext != nullptr) {
        wglDeleteContext(openglContext);  // Delete the OpenGL rendering context
        openglContext = nullptr;
    }

    if (deviceContext != nullptr) {
        ReleaseDC(GetConsoleWindow(), deviceContext);  // Release the device context
        deviceContext = nullptr;
    }

    DestroyWindow(hwnd);
#endif
}

bool CrossPlatformWindow::createContext(int majorVersion, int minorVersion) {
#ifdef _WIN32
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
    if (pixelFormat == 0 || !SetPixelFormat(deviceContext, pixelFormat, &pfd)) {
        MessageBoxA(hwnd, "Failed to set pixel format", "OpenGL Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Create a dummy legacy context to load wgl extensions
    HGLRC dummyContext = wglCreateContext(deviceContext);
    if (!dummyContext || !wglMakeCurrent(deviceContext, dummyContext)) {
        MessageBoxA(hwnd, "Failed to create dummy OpenGL context", "OpenGL Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Load WGL extensions
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
        (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (!wglCreateContextAttribsARB) {
        MessageBoxA(hwnd, "wglCreateContextAttribsARB not available", "OpenGL Error", MB_OK | MB_ICONERROR);
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(dummyContext);
        return false;
    }

    // Done with dummy.
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(dummyContext);

    // Create real modern OpenGL context.
    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
        WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    openglContext = wglCreateContextAttribsARB(deviceContext, nullptr, attribs);
    if (!openglContext || !wglMakeCurrent(deviceContext, openglContext)) {
        MessageBoxA(hwnd, "Failed to create modern OpenGL context", "OpenGL Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
#else
    return false;
#endif
}

void CrossPlatformWindow::setContext() {
#ifdef _WIN32
    wglMakeCurrent(deviceContext, openglContext);
#endif
}

void CrossPlatformWindow::swapBuffers() {
#ifdef _WIN32
    SwapBuffers(deviceContext);
#endif
}

void CrossPlatformWindow::pollEvents() {
#ifdef _WIN32
    MSG msg = {};
    // Check if there is a message in the queue.
    if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        // Translate and dispatch the message.
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return;
    }
#endif
}

#ifdef _WIN32
LRESULT CrossPlatformWindow::Win32WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Windows allows the user to save custom data thats tied to the hwnd.
    // This can be done by calling SetWindowLongPtr and retrived as below
    // using GetWindowLonPtr.
    auto *window = reinterpret_cast<CrossPlatformWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));;

    // Continue if the window has not been setup yet.
    if (!window) return DefWindowProc(hwnd, msg, wParam, lParam);

    if (msg == WM_CLOSE) {
        window->isCurrentlyOpen = false;
    }

    if (msg == WM_SIZE) {
        window->width = LOWORD(lParam);
        window->height = HIWORD(lParam);
    }

    /* WHEN ANOTHER WINDOW IS SELECTED */
    if (msg == WM_KILLFOCUS) {
        window->isKeyboardFocused = false;

        memset(window->mouse.buttons, 0, sizeof window->mouse.buttons);
        memset(window->keybaord.keysPressed, false, sizeof(window->keybaord.keysPressed));
        memset(window->keybaord.keysToggled, false, sizeof(window->keybaord.keysToggled));
    }
    if (msg == WM_SETFOCUS) {
        window->isKeyboardFocused = true;
    }


    /* KEY INPUT */
    if (msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP || msg == WM_KEYDOWN || msg == WM_KEYUP) {
        if (window->isKeyboardFocused) {
            bool keyIsDown = ((lParam  &(1 << 31)) == 0);   // Key is down if bit 31 is 0
            bool keyWasDown = ((lParam  &(1 << 30)) != 0);   // Key was down if bit 30 is 1


            // Store state in `keyboard` for regular keys
            if (keyIsDown != keyWasDown) {

                if (wParam >= 'A' & wParam < 'z') {

                    window->keybaord.keysPressed[wParam - ((uint8_t) 'A')] = keyIsDown;

                    if (keyIsDown & !keyWasDown) {
                        window->keybaord.keysToggled[wParam - ((uint8_t) 'A')] ^= true;

                    }
                }

                switch (wParam) {
                    case VK_SPACE: {
                        window->keybaord.keysPressed[KEY_VAL_SPACE] ^= true;
                        if (keyIsDown & !keyWasDown)
                            window->keybaord.keysToggled[KEY_VAL_SPACE] ^= keyIsDown;
                    } break;
                    case VK_SHIFT:
                    case VK_LSHIFT: {
                        window->keybaord.keysPressed[KEY_VAL_LSHIFT] = keyIsDown;
                        if (keyIsDown & !keyWasDown) {
                            window->keybaord.keysToggled[KEY_VAL_LSHIFT] ^= true;
                        }
                    } break;
                    case VK_RSHIFT: {
                        window->keybaord.keysPressed[KEY_VAL_RSHIFT] = keyIsDown;
                        if (keyIsDown & !keyWasDown) {
                            window->keybaord.keysToggled[KEY_VAL_RSHIFT] ^= true;
                        }
                    } break;
                    case VK_CONTROL:
                    case VK_LCONTROL: {
                        window->keybaord.keysPressed[KEY_VAL_LCTRL] = keyIsDown;
                        if (keyIsDown & !keyWasDown) {
                            window->keybaord.keysToggled[KEY_VAL_LCTRL] ^= true;
                        }
                    } break;
                    case VK_RCONTROL: {
                        window->keybaord.keysPressed[KEY_VAL_RCTRL] = keyIsDown;
                        if (keyIsDown & !keyWasDown) {
                            window->keybaord.keysToggled[KEY_VAL_RCTRL] ^= true;
                        }
                    } break;

                    case VK_TAB: {
                        window->keybaord.keysPressed[KEY_VAL_TAB] = keyIsDown;
                        if (keyIsDown & !keyWasDown) {
                            window->keybaord.keysToggled[KEY_VAL_TAB] ^= true;
                        }
                    } break;
                }
            }
        }
    }

    if (msg == WM_MOUSEMOVE) {
        POINT pos;
        GetCursorPos(&pos);
        ScreenToClient(window->hwnd, &pos);
        window->mouse.x = pos.x;
        window->mouse.y = pos.y;
    }

    if (msg == WM_LBUTTONDOWN) {
        if (GetFocus() != hwnd) {
            SetFocus(window->hwnd);
        }
        window->mouse.buttons[MOUSE_LEFT] = true;
    }
    if (msg == WM_LBUTTONUP) {
        window->mouse.buttons[MOUSE_LEFT] = false;
    }

    if (msg == WM_MBUTTONDOWN) {
        window->mouse.buttons[MOUSE_MIDDLE] = true;
    }

    if (msg == WM_MBUTTONUP) {
        window->mouse.buttons[MOUSE_MIDDLE] = false;
    }

    if (msg == WM_RBUTTONDOWN) {
        window->mouse.buttons[MOUSE_RIGHT] = true;
    }

    if (msg == WM_RBUTTONUP) {
        window->mouse.buttons[MOUSE_RIGHT] = false;
    }

    /* GET THE MOVEMENT OF THE MOUSE THIS FRAME */
    window->mouse.deltaX = window->mouse.x - window->mouse.oldMouseX;
    window->mouse.deltaY = window->mouse.y - window->mouse.oldMouseY;

    window->mouse.oldMouseX = window->mouse.x;
    window->mouse.oldMouseY = window->mouse.y;

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif
