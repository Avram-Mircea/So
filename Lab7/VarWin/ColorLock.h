#pragma once
#include <windows.h>

class ColorLock {
public:
    ColorLock();

    void white_enter();
    void white_exit();

    void black_enter();
    void black_exit();

private:
    CRITICAL_SECTION m;
    CONDITION_VARIABLE okWhite;
    CONDITION_VARIABLE okBlack;

    int activeWhite;
    int activeBlack;
    int waitingWhite;
    int waitingBlack;

    enum Color { NONE, WHITE, BLACK };
    Color currentColor;
};
