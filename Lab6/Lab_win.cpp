#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

struct SharedData { int number; };

void runWriter(SharedData* data, HANDLE sem);
void runReader(SharedData* data, HANDLE sem);

int main(int argc, char* argv[]) {

    // ======================================================
    // =============== CHILD PROCESS MODE ===================
    // ======================================================
    if (argc == 2 && std::string(argv[1]) == "writer") {

        HANDLE hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "Local\\SharedMemoryDemo");
        HANDLE hSem = OpenSemaphoreA(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, "Local\\SemaphoreDemo");

        SharedData* data = (SharedData*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));

        runWriter(data, hSem);
        return 0;
    }

    if (argc == 2 && std::string(argv[1]) == "reader") {

        HANDLE hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "Local\\SharedMemoryDemo");
        HANDLE hSem = OpenSemaphoreA(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, "Local\\SemaphoreDemo");

        SharedData* data = (SharedData*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));

        runReader(data, hSem);
        return 0;
    }

    // ======================================================
    // ================= PARENT PROCESS MODE ================
    // ======================================================

    std::cout << "[PARENT] Starting shared memory + sem + processes...\n";

    HANDLE hMap = CreateFileMappingA(
        INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
        sizeof(SharedData), "Local\\SharedMemoryDemo");

    if (!hMap) { std::cout << "CreateFileMapping failed\n"; return 1; }

    SharedData* data = (SharedData*)MapViewOfFile(
        hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));

    HANDLE sem = CreateSemaphoreA(NULL, 1, 1, "Local\\SemaphoreDemo");
    if (!sem) { std::cout << "Semaphore fail\n"; return 1; }

    // ------------------------------------------------------
    // LAUNCH writer
    // ------------------------------------------------------
    STARTUPINFOA si1{};
    PROCESS_INFORMATION pi1{};
    si1.cb = sizeof(si1);

    BOOL ok1 = CreateProcessA(
        NULL,
        (LPSTR)"Lab_win.exe writer",
        NULL, NULL, FALSE, 0, NULL, NULL,
        &si1, &pi1
    );

    if (!ok1) {
        std::cout << "Failed to start writer. Error=" << GetLastError() << "\n";
        return 1;
    }

    // ------------------------------------------------------
    // LAUNCH reader
    // ------------------------------------------------------
    STARTUPINFOA si2{};
    PROCESS_INFORMATION pi2{};
    si2.cb = sizeof(si2);

    BOOL ok2 = CreateProcessA(
        NULL,
        (LPSTR)"Lab_win.exe reader",
        NULL, NULL, FALSE, 0, NULL, NULL,
        &si2, &pi2
    );

    if (!ok2) {
        std::cout << "Failed to start reader. Error=" << GetLastError() << "\n";
        return 1;
    }

    // ------------------------------------------------------
    // WAIT FOR BOTH CHILDREN
    // ------------------------------------------------------
    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);

    std::cout << "[PARENT] Both finished.\n";

    CloseHandle(pi1.hThread);
    CloseHandle(pi1.hProcess);
    CloseHandle(pi2.hThread);
    CloseHandle(pi2.hProcess);

    return 0;
}


// ==========================================================
// =================== CHILD LOGIC ==========================
// ==========================================================

void runWriter(SharedData* data, HANDLE sem) {
    srand(time(NULL) + 1);

    data->number = 1;

    while (data->number <= 1000) {
        WaitForSingleObject(sem, INFINITE);

        std::cout << "[Writer] sees " << data->number << "\n";

        if (rand() % 2 == 1) {
            data->number++;
            std::cout << "[Writer] wrote " << data->number << "\n";
        }
        else std::cout << "[Writer] PASS\n";

        ReleaseSemaphore(sem, 1, NULL);
        Sleep(10);
    }
}

void runReader(SharedData* data, HANDLE sem) {
    srand(time(NULL) + 2);

    while (data->number <= 1000) {
        WaitForSingleObject(sem, INFINITE);

        std::cout << "[Reader] sees " << data->number << "\n";

        if (rand() % 2 == 1) {
            data->number++;
            std::cout << "[Reader] wrote " << data->number << "\n";
        }
        else std::cout << "[Reader] PASS\n";

        ReleaseSemaphore(sem, 1, NULL);
        Sleep(10);
    }
}
