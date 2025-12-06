#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include "ColorLock.h"

using namespace std;

#define wireNumber 2
#define iterations 3

ColorLock colock;
mutex out;

void whiteThread(int id) {
    for (int i = 0; i < iterations; i++) {
        this_thread::sleep_for(chrono::milliseconds(50));

        colock.white_enter();
        {
            lock_guard<mutex> lk(out);
            cout << "White thread " << id << " in (" << i + 1 << ")\n";
        }

        this_thread::sleep_for(chrono::milliseconds(100));

        colock.white_exit();
        {
            lock_guard<mutex> lk(out);
            cout << "White thread " << id << " out (" << i + 1 << ")\n";
        }
    }
}

void blackThread(int id) {
    for (int i = 0; i < iterations; i++) {
        this_thread::sleep_for(chrono::milliseconds(50));

        colock.black_enter();
        {
            lock_guard<mutex> lk(out);
            cout << "\033[34m"; // albastru
            cout << "Black thread " << id << " in (" << i + 1 << ")\n";
            cout << "\033[0m";  // reset
        }

        this_thread::sleep_for(chrono::milliseconds(100));

        colock.black_exit();
        {
            lock_guard<mutex> lk(out);
            cout << "\033[34m"; // albastru
            cout << "Black thread " << id << " out (" << i + 1 << ")\n";
            cout << "\033[0m";  // reset
        }
    }
}

int main() {
    vector<thread> threads;

    for (int i = 0; i < wireNumber; i++) {
        threads.push_back(thread(whiteThread, i + 1));
        threads.push_back(thread(blackThread, i + 1));
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
