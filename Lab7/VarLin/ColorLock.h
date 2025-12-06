#pragma once
#include <mutex>
#include <condition_variable>

class ColorLock {
public:
    ColorLock() : activeWhite(0), activeBlack(0), waitingWhite(0), waitingBlack(0), currentColor(NONE) {}

    void white_enter();
    void white_exit();

    void black_enter();
    void black_exit();

private:
    std::mutex mtx;
    std::condition_variable cvWhite;
    std::condition_variable cvBlack;

    int activeWhite;
    int activeBlack;
    int waitingWhite;
    int waitingBlack;

    enum Color { NONE, WHITE, BLACK };
    Color currentColor;
};
