#include "ColorLock.h"
#include <iostream>
#include <cstdlib>

void ColorLock::white_enter() {
    std::unique_lock<std::mutex> lk(mtx);
    waitingWhite++;

    while (currentColor == BLACK || (waitingBlack > 0 && currentColor != WHITE)) {
        cvWhite.wait(lk);
    }

    waitingWhite--;
    currentColor = WHITE;

    if (activeBlack > 0) {
        std::cerr << "ERROR: white entered while black active!\n";
        std::exit(1);
    }

    activeWhite++;
}

void ColorLock::white_exit() {
    std::unique_lock<std::mutex> lk(mtx);
    activeWhite--;

    if (activeWhite == 0) {
        if (waitingBlack > 0) {
            currentColor = BLACK;
            cvBlack.notify_all();
        }
        else {
            currentColor = NONE;
        }
    }
}

void ColorLock::black_enter() {
    std::unique_lock<std::mutex> lk(mtx);
    waitingBlack++;

    while (currentColor == WHITE || (waitingWhite > 0 && currentColor != BLACK)) {
        cvBlack.wait(lk);
    }

    waitingBlack--;
    currentColor = BLACK;

    if (activeWhite > 0) {
        std::cerr << "ERROR: black entered while white active!\n";
        std::exit(1);
    }

    activeBlack++;
}

void ColorLock::black_exit() {
    std::unique_lock<std::mutex> lk(mtx);
    activeBlack--;

    if (activeBlack == 0) {
        if (waitingWhite > 0) {
            currentColor = WHITE;
            cvWhite.notify_all();
        }
        else {
            currentColor = NONE;
        }
    }
}
