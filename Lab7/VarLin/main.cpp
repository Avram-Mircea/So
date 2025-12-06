#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "ColorLock.h"
#include <mutex>

using namespace std;

#define wireNumber 2

mutex out;
ColorLock colock;

void* whiteThread(void* param) {
    int id = *(int*)param;

    for (int i = 0; i < 3; i++) {
        colock.white_enter();

        {
            lock_guard<mutex> lk(out);
            cout << "White thread " << id << " in (" << i + 1 << ")\n";
        }

        usleep(200000); // 200ms

        colock.white_exit();

        {
            lock_guard<mutex> lk(out);
            cout << "White thread " << id << " out (" << i + 1 << ")\n";
        }
    }

    return nullptr;
}


void* blackThread(void* param) {
    int id = *(int*)param;

    for (int i = 0; i < 3; i++) {
        colock.black_enter();

        {
            lock_guard<mutex> lk(out);
            // Pentru Linux terminal color: albastru pentru Black thread
            cout << "\033[34m"; // escape code pentru text albastru
            cout << "Black thread " << id << " in (" << i + 1 << ")\n";
            cout << "\033[0m";  // reset culoare
        }

        usleep(200000); // 200ms

        colock.black_exit();

        {
            lock_guard<mutex> lk(out);
            cout << "\033[34m"; // albastru
            cout << "Black thread " << id << " out (" << i + 1 << ")\n";
            cout << "\033[0m";  // reset culoare
        }
    }

    return nullptr;
}


int main() {
    pthread_t whiteWires[wireNumber];
    pthread_t blackWires[wireNumber];

    int whiteID[wireNumber];
    int blackID[wireNumber];

    for (int i = 0; i < wireNumber; i++) {
        whiteID[i] = i + 1;
        pthread_create(&whiteWires[i], nullptr, whiteThread, &whiteID[i]);
    }

    for (int i = 0; i < wireNumber; i++) {
        blackID[i] = i + 1;
        pthread_create(&blackWires[i], nullptr, blackThread, &blackID[i]);
    }

    for (int i = 0; i < wireNumber; i++) {
        pthread_join(whiteWires[i], nullptr);
        pthread_join(blackWires[i], nullptr);
    }

    return 0;
}
