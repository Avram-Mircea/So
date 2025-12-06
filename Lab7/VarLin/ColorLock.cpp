#include "ColorLock.h"
#include <iostream>
#include <cstdlib>

ColorLock::ColorLock() :
    activeWhite(0), activeBlack(0),
    waitingWhite(0), waitingBlack(0),
    currentColor(NONE)
{
    if (pthread_mutex_init(&m, nullptr) != 0) {
        std::cerr << "Mutex init failed\n";
        std::exit(1);
    }
    if (pthread_cond_init(&okWhite, nullptr) != 0) {
        std::cerr << "Condition variable init failed\n";
        std::exit(1);
    }
    if (pthread_cond_init(&okBlack, nullptr) != 0) {
        std::cerr << "Condition variable init failed\n";
        std::exit(1);
    }
}

ColorLock::~ColorLock() {
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&okWhite);
    pthread_cond_destroy(&okBlack);
}


void ColorLock::white_enter() {
    pthread_mutex_lock(&m);
    waitingWhite++;

    // Firul alb asteapta cat timp:
    // - exista deja fire negre active sau
    // - exista fire negre care asteapta si nu e inca randul albelor
    while (currentColor == BLACK || (waitingBlack > 0 && currentColor != WHITE)) {
        pthread_cond_wait(&okWhite, &m);
    }

    waitingWhite--;
    currentColor = WHITE;

    if (activeBlack > 0) { // verificam ca nu intra peste ele
        std::cerr << "ERROR: white entered while black active!\n";
        std::exit(1);
    }

    activeWhite++;
    pthread_mutex_unlock(&m);
}

void ColorLock::white_exit() {
    pthread_mutex_lock(&m);

    activeWhite--;

    if (activeWhite == 0) { // toate firele albe au terminat
        if (waitingBlack > 0) { // exista fire negre care asteapta
            currentColor = BLACK; // e randul negrelor
            pthread_cond_broadcast(&okBlack); // instintam toate firele negre ca pot intra
        }
        else {
            currentColor = NONE; // toate firele au terminat
        }
    }

    pthread_mutex_unlock(&m);
}


void ColorLock::black_enter() {
    pthread_mutex_lock(&m);
    waitingBlack++;

    // Firul negru asteapta cat timp:
    // - exista deja fire albe active sau
    // - exista fire albe care asteapta si nu e inca randul negrelor
    while (currentColor == WHITE || (waitingWhite > 0 && currentColor != BLACK)) {
        pthread_cond_wait(&okBlack, &m);
    }

    waitingBlack--;
    currentColor = BLACK;

    if (activeWhite > 0) { // verificam ca nu intra peste ele
        std::cerr << "ERROR: black entered while white active!\n";
        std::exit(1);
    }

    activeBlack++;
    pthread_mutex_unlock(&m);
}

void ColorLock::black_exit() {
    pthread_mutex_lock(&m);

    activeBlack--;

    if (activeBlack == 0) { // toate firele negre au terminat
        if (waitingWhite > 0) { // exista fire albe care asteapta
            currentColor = WHITE; // e randul albelor
            pthread_cond_broadcast(&okWhite); // instintam toate firele albe ca pot intra
        }
        else {
            currentColor = NONE; // toate firele au terminat
        }
    }

    pthread_mutex_unlock(&m);
}
