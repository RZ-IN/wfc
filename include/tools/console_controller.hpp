/*
 * ConsoleController.hpp
 * Created on 2025.04.25 by DeepSeek
 * Edited on 2025.04.25 by RZIN
 */
#pragma once
#include <windows.h>
namespace cha
{



class ConsoleController {
private:
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

public:
    ConsoleController() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
    }

    // 设置光标位置
    void SetCursor(int x, int y) {
        COORD coord = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(hConsole, coord);
    }

    // 获取当前光标X坐标
    int GetCursorX() {
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        return csbi.dwCursorPosition.X;
    }

    // 获取当前光标Y坐标
    int GetCursorY() {
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        return csbi.dwCursorPosition.Y;
    }

    // 隐藏/显示光标
    void ShowCursor(bool show) {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = show;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }
};



} // namespace cha