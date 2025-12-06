#include "ColorLock.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void ColorLock::white_enter() {
    unique_lock<mutex> lk(mtx);
    waitingWhite++;

    // Firul alb asteapta cat timp:
    // -exista deja fire negre active sau
    // -exista fire negre care asteapta si nu e inca randul albelor
    while (currentColor == BLACK || (waitingBlack > 0 && currentColor != WHITE)) {
        cvWhite.wait(lk);
    }

    waitingWhite--;
    currentColor = WHITE;

    if (activeBlack > 0) { // verificam ca nu intra peste ele
        cerr << "ERROR: white entered while black active!\n";
        exit(1);
    }

    activeWhite++;
}

void ColorLock::white_exit() {
    unique_lock<mutex> lk(mtx);
    activeWhite--;

    if (activeWhite == 0) { // toate firele albe au terminat
        if (waitingBlack > 0) { // exista fire negre care asteapta
            currentColor = BLACK; // e randul negrelor
            cvBlack.notify_all(); // instintam toate firele negre ca pot intra
        }
        else {
            currentColor = NONE; // toate firele au terminat
        }
    }
}

void ColorLock::black_enter() {
    unique_lock<mutex> lk(mtx);
    waitingBlack++;

    // Firul negru asteapta cat timp:
    // -exista deja fire albe active sau
    // -exista fire albe care asteapta si nu e inca randul negrelor
    while (currentColor == WHITE || (waitingWhite > 0 && currentColor != BLACK)) {
        cvBlack.wait(lk);
    }

    waitingBlack--;
    currentColor = BLACK;

    if (activeWhite > 0) { // verificam ca nu intra peste ele
        cerr << "ERROR: black entered while white active!\n";
        exit(1);
    }

    activeBlack++;
}

void ColorLock::black_exit() {
    unique_lock<mutex> lk(mtx);
    activeBlack--;

    if (activeBlack == 0) { // toate firele negre au terminat
        if (waitingWhite > 0) { // exista fire albe care asteapta
            currentColor = WHITE; // e randul albelor
            cvWhite.notify_all(); // instintam toate firele albe ca pot intra
        }
        else {
            currentColor = NONE; // toate firele au terminat
        }
    }
}
