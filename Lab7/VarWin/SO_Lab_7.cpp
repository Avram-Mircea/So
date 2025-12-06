#include <windows.h>
#include <iostream>
#include "ColorLock.h"
#include <mutex>

#define wireNumber 2

using namespace std;

mutex out;

ColorLock colock;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

DWORD WINAPI whiteThread(LPVOID param) {
    int id = *(int*)param;

    for (int i = 0; i < 3; i++) {
        colock.white_enter();
        {
            lock_guard<mutex> lk(out);
            cout << "White thread " << id << " in (" << i + 1 << ")\n";
        }

        Sleep(200);

        colock.white_exit();
        {
            lock_guard<mutex> lk(out);
            cout << "White thread " << id << " out (" << i + 1 << ")\n";
        }
    }
    return 0;
}

DWORD WINAPI blackThread(LPVOID param) {
    int id = *(int*)param;

    for (int i = 0; i < 3; i++) {
        colock.black_enter();

        {
            lock_guard<mutex> lk(out);
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); //albastru
            cout << "Black thread " << id << " in (" << i + 1 << ")\n";
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //alb
        }

        Sleep(200);

        colock.black_exit();

        {
            lock_guard<mutex> lk(out);
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); //albastru
            cout << "Black thread " << id << " out (" << i + 1 << ")\n";
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); //alb
        }
    }
    return 0;
}

int main()
{
    HANDLE whiteWires[wireNumber];
    HANDLE blackWires[wireNumber];

    int whiteID[wireNumber];
    int blackeID[wireNumber];

    for (int i = 0; i < wireNumber; i++) {
        whiteID[i] = i + 1;
        whiteWires[i] = CreateThread(NULL, 0, whiteThread, &whiteID[i], 0, NULL);
    }

    for (int i = 0; i < wireNumber; i++) {
        blackeID[i] = i + 1;
        blackWires[i] = CreateThread(NULL, 0, blackThread, &blackeID[i], 0, NULL);
    }

    WaitForMultipleObjects(wireNumber, whiteWires, TRUE, INFINITE);

    WaitForMultipleObjects(wireNumber, blackWires, TRUE, INFINITE);

    for(int i = 0; i < wireNumber; i++) {
        CloseHandle(whiteWires[i]);
        CloseHandle(blackWires[i]);
    }

    return 0;
}
