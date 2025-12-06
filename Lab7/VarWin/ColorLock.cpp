#include "ColorLock.h"

ColorLock::ColorLock() :
    activeWhite(0), activeBlack(0),
    waitingWhite(0), waitingBlack(0),
    currentColor(NONE)
{
    InitializeCriticalSection(&m);
    InitializeConditionVariable(&okWhite);
    InitializeConditionVariable(&okBlack);
}

void ColorLock::white_enter()
{
    EnterCriticalSection(&m);
    waitingWhite++;

    // Firul alb asteapta cat timp:
    // -exista deja fire negre active sau
    // -exista fire negre care asteapta si nu e inca randul albelor
    while (currentColor == BLACK || (waitingBlack > 0 && currentColor != WHITE))
    {
        SleepConditionVariableCS(&okWhite, &m, INFINITE);
    }

    waitingWhite--;
    currentColor = WHITE;

    if (activeBlack > 0) { // verificam ca nu intra peste ele
        MessageBoxA(0, "ERROR: white entered while black active!", "Bug", 0);
    }

    activeWhite++;

    LeaveCriticalSection(&m);
}

void ColorLock::white_exit()
{
    EnterCriticalSection(&m);

    activeWhite--;

    if (activeWhite == 0) { // toate firele albe au terminat
        if (waitingBlack > 0) { // exista fire negre care asteapta
            currentColor = BLACK; // e randul negrelor
            WakeAllConditionVariable(&okBlack); // instintam toate firele negre ca pot intra
        }
        else {
            currentColor = NONE; // toate firele au terminat
        }
    }

    LeaveCriticalSection(&m);
}

void ColorLock::black_enter()
{
    EnterCriticalSection(&m);
    waitingBlack++;

    // Firul negru asteapta cat timp:
    // -exista deja fire albe active sau
    // -exista fire albe care asteapta si nu e inca randul negrelor
    while (currentColor == WHITE || (waitingWhite > 0 && currentColor != BLACK))
    {
        SleepConditionVariableCS(&okBlack, &m, INFINITE);
    }

    waitingBlack--;
    currentColor = BLACK;

    if (activeWhite > 0) { // verificam ca nu intra peste ele
        MessageBoxA(0, "ERROR: black entered while white active!", "Bug", 0);
    }

    activeBlack++;

    LeaveCriticalSection(&m);
}

void ColorLock::black_exit()
{
    EnterCriticalSection(&m);

    activeBlack--;

    if (activeBlack == 0) { // toate firele negre au terminat
        if (waitingWhite > 0) { // exista fire albe care asteapta
            currentColor = WHITE; // e randul albelor
            WakeAllConditionVariable(&okWhite); // instintam toate firele albe ca pot intra
        }
        else {
            currentColor = NONE; // toate firele au terminat
        }
    }

    LeaveCriticalSection(&m);
}
