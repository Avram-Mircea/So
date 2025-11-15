#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

int main(int argc, char* argv[]) {

    // ----------------------------------------
    // ---------- CHILD PROCESS CODE ----------
    // ----------------------------------------
    if (argc == 4 && string(argv[1]) == "--child") {
        int start = stoi(argv[2]);
        int end = stoi(argv[3]);

        vector<int> primes;
        for (int n = start; n <= end; n++)
            if (isPrime(n)) primes.push_back(n);

        int size = primes.size();

        // Write size then primes to stdout
        DWORD written;
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),
            &size, sizeof(int), &written, NULL);

        if (size > 0)
            WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),
                primes.data(), sizeof(int) * size, &written, NULL);

        return 0;
    }

    // ----------------------------------------
    // ---------- PARENT PROCESS CODE ---------
    // ----------------------------------------

    const int PROC = 10;
    HANDLE hRead[PROC], hWrite[PROC];

    for (int i = 0; i < PROC; i++) {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;

        // Create pipe for child -> parent
        CreatePipe(&hRead[i], &hWrite[i], &sa, 0);
        SetHandleInformation(hRead[i], HANDLE_FLAG_INHERIT, 0);

        int start = i * 1000 + 1;
        int end = (i + 1) * 1000;

        // Complete command including filename
        string cmd = "Lab5_SO.exe --child " +
            to_string(start) + " " + to_string(end);

        STARTUPINFOA si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);
        si.hStdOutput = hWrite[i];  // child's stdout pipe
        si.hStdError = hWrite[i];
        si.dwFlags |= STARTF_USESTDHANDLES;

        vector<char> cmdBuffer(cmd.begin(), cmd.end());
        cmdBuffer.push_back('\0');

        BOOL ok = CreateProcessA(
            NULL,
            cmdBuffer.data(),   // now correct type LPSTR
            NULL, NULL,
            TRUE,
            0,
            NULL, NULL,
            &si,
            &pi
        );

        if (!ok) {
            cerr << "Eroare la CreateProcess pentru procesul " << i << endl;
            return 1;
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        CloseHandle(hWrite[i]); // parent closes write end
    }

    // ------ read results from children ------
    for (int i = 0; i < PROC; i++) {

        DWORD readBytes;
        int size = 0;

        ReadFile(hRead[i], &size, sizeof(int), &readBytes, NULL);

        if (readBytes != sizeof(int)) {
            cout << "Proces " << i << ": NU a trimis date.\n";
            continue;
        }

        vector<int> primes(size);

        if (size > 0) {
            ReadFile(hRead[i], primes.data(),
                sizeof(int) * size, &readBytes, NULL);
        }

        cout << "Proces " << i << ": ";
        for (int p : primes)
            cout << p << " ";
        cout << "\n\n";

        CloseHandle(hRead[i]);
    }

    return 0;
}
