#ifndef CROSS_PLATFORM_WINDOW_HPP
#define CROSS_PLATFORM_WINDOW_HPP

#include "keys.hpp"

#include <glad/glad.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/wgl.h>
#endif

// STD
#include <string>

class CrossPlatformWindow {
public:
    enum MouseButtons {
        MOUSE_LEFT = 0,
        MOUSE_MIDDLE,
        MOUSE_RIGHT,
        MOUSE_COUNT,
    };

    struct Mouse {
        int x;
        int y;
        int deltaX;
        int deltaY;
        int oldMouseX;
        int oldMouseY;
        bool buttons[MOUSE_COUNT];
    };

    typedef bool Key;
    struct Keybaord {
        Key keysPressed[WINDOW_KEYS_COUNT];
        Key keysToggled[WINDOW_KEYS_COUNT];
    };

    CrossPlatformWindow(int width, int height, const std::string &windowTitle);
    ~CrossPlatformWindow();


    bool createContext(int majorVersion, int minorVersion);
    void setContext();
    void swapBuffers();
    void pollEvents();
    bool isWindowOpen() { return isCurrentlyOpen; }
    int getWidth() { return width; }
    int getHeight() { return height; }
    Key getKeyPresssed(int key) { return keybaord.keysPressed[key]; }
    Key getKeyToggled(int key) { return keybaord.keysToggled[key]; }
    const Mouse &getMouse() { return mouse; }
private:
    int width = 200;
    int height = 200;
    std::string title;

    bool isCurrentlyOpen = false;
    bool isKeyboardFocused = false;

    Mouse mouse = {};
    Keybaord keybaord;
#ifdef _WIN32
    HWND hwnd;

    HGLRC openglContext;
    HDC deviceContext;

    static LRESULT CALLBACK Win32WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
};

#endif
