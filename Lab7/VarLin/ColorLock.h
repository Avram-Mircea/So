#pragma once
#include <pthread.h>

class ColorLock {
public:
    ColorLock();
    ~ColorLock();

    void white_enter();
    void white_exit();

    void black_enter();
    void black_exit();

private:
    pthread_mutex_t m;
    pthread_cond_t okWhite;
    pthread_cond_t okBlack;

    int activeWhite;
    int activeBlack;
    int waitingWhite;
    int waitingBlack;

    enum Color { NONE, WHITE, BLACK };
    Color currentColor;
};
