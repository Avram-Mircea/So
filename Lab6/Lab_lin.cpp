#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <sys/wait.h>

struct SharedData { int number; };

void runWriter(SharedData* data, sem_t* sem);
void runReader(SharedData* data, sem_t* sem);

int main() {
    srand(time(nullptr));

    // Creare / deschidere shared memory
    int fd = shm_open("/shm_demo", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(SharedData));
    SharedData* data = (SharedData*) mmap(nullptr, sizeof(SharedData),
                                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Creare / deschidere semafor
    sem_t* sem = sem_open("/sem_demo", O_CREAT, 0666, 1);

    data->number = 1; // initializare

    pid_t writer_pid = fork();
    if (writer_pid == 0) {
        runWriter(data, sem);
        munmap(data, sizeof(SharedData));
        close(fd);
        return 0;
    }

    pid_t reader_pid = fork();
    if (reader_pid == 0) {
        runReader(data, sem);
        munmap(data, sizeof(SharedData));
        close(fd);
        return 0;
    }

    // Parintele asteapta copiii
    waitpid(writer_pid, nullptr, 0);
    waitpid(reader_pid, nullptr, 0);

    // Curatare
    munmap(data, sizeof(SharedData));
    close(fd);
    shm_unlink("/shm_demo");
    sem_close(sem);
    sem_unlink("/sem_demo");

    std::cout << "[PARENT] Finished.\n";
    return 0;
}

// ================= CHILD LOGIC =================

void runWriter(SharedData* data, sem_t* sem) {
    srand(time(nullptr) + 1);

    while (data->number <= 1000) {
        sem_wait(sem);

        std::cout << "[Writer] sees " << data->number << "\n";

        if (rand() % 2 == 1) {
            data->number++;
            std::cout << "[Writer] wrote " << data->number << "\n";
        } else {
            std::cout << "[Writer] PASS\n";
        }

        sem_post(sem);
        usleep(10000);
    }
}

void runReader(SharedData* data, sem_t* sem) {
    srand(time(nullptr) + 2);

    while (data->number <= 1000) {
        sem_wait(sem);

        std::cout << "[Reader] sees " << data->number << "\n";

        if (rand() % 2 == 1) {
            data->number++;
            std::cout << "[Reader] wrote " << data->number << "\n";
        } else {
            std::cout << "[Reader] PASS\n";
        }

        sem_post(sem);
        usleep(10000);
    }
}

