#include <iostream>
#include <windows.h>
#include <process.h>
#include <cstdlib>

#define BUFFER_SIZE 20

// Shared memory structure
struct SharedMemory {
    int buffer[BUFFER_SIZE]; // Circular buffer
    int in;                  // Index for producer to insert
    int out;                 // Index for consumer to remove
    int SUM;                 // Shared sum variable
    int totalProduced;       // Total numbers produced by all producers
    int m;                   // Number of producers
};

// Global variables
SharedMemory* shm = nullptr;
HANDLE hMapFile;            // Handle for shared memory
HANDLE emptySem, fullSem, mutexSem; // Semaphores and mutex

// Producer function
void producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 1; i <= 100; ++i) {
        WaitForSingleObject(emptySem, INFINITE); // Wait for empty slot
        WaitForSingleObject(mutexSem, INFINITE); // Lock mutex

        // Critical section: Insert into buffer
        shm->buffer[shm->in] = i;
        shm->in = (shm->in + 1) % BUFFER_SIZE;
        shm->totalProduced++;
        std::cout << "Producer " << id << " produced: " << i << std::endl;

        ReleaseMutex(mutexSem); // Unlock mutex
        ReleaseSemaphore(fullSem, 1, NULL); // Signal full slot
    }
    _endthread();
}

// Consumer function
void consumer(void* arg) {
    int id = *(int*)arg;
    while (true) {
        WaitForSingleObject(fullSem, INFINITE); // Wait for full slot
        WaitForSingleObject(mutexSem, INFINITE); // Lock mutex

        // Critical section: Read from buffer
        int item = shm->buffer[shm->out];
        shm->out = (shm->out + 1) % BUFFER_SIZE;
        shm->SUM += item;
        std::cout << "Consumer " << id << " consumed: " << item << std::endl;

        ReleaseMutex(mutexSem); // Unlock mutex
        ReleaseSemaphore(emptySem, 1, NULL); // Signal empty slot

        if (shm->SUM >= shm->totalProduced && shm->totalProduced == 100 * shm->m) { // All items consumed
            break;
        }
    }
    _endthread();
}

int main() {
    int m, n;
    std::cout << "Enter number of producers (m): ";
    std::cin >> m;
    std::cout << "Enter number of consumers (n): ";
    std::cin >> n;

    // Create shared memory
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMemory), NULL);
    if (hMapFile == NULL) {
        std::cerr << "Failed to create shared memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    shm = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
    shm->in = 0;
    shm->out = 0;
    shm->SUM = 0;
    shm->totalProduced = 0;
    shm->m = m; // Store m in shared memory

    // Create semaphores and mutex
    emptySem = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL); // Tracks empty slots
    fullSem = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);           // Tracks filled slots
    mutexSem = CreateMutex(NULL, FALSE, NULL);                       // Ensures mutual exclusion

    if (emptySem == NULL || fullSem == NULL || mutexSem == NULL) {
        std::cerr << "Failed to create synchronization objects" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Fork producers
    HANDLE* producerHandles = new HANDLE[m];
    int* producerIds = new int[m];
    for (int i = 0; i < m; ++i) {
        producerIds[i] = i + 1;
        producerHandles[i] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall*)(void*))producer, &producerIds[i], 0, NULL);
        if (producerHandles[i] == NULL) {
            std::cerr << "Failed to create producer thread" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Fork consumers
    HANDLE* consumerHandles = new HANDLE[n];
    int* consumerIds = new int[n];
    for (int i = 0; i < n; ++i) {
        consumerIds[i] = i + 1;
        consumerHandles[i] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall*)(void*))consumer, &consumerIds[i], 0, NULL);
        if (consumerHandles[i] == NULL) {
            std::cerr << "Failed to create consumer thread" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all producers and consumers to finish
    WaitForMultipleObjects(m, producerHandles, TRUE, INFINITE);
    WaitForMultipleObjects(n, consumerHandles, TRUE, INFINITE);

    // Print final SUM
    std::cout << "Final SUM: " << shm->SUM << std::endl;

    // Cleanup
    UnmapViewOfFile(shm);
    CloseHandle(hMapFile);
    CloseHandle(emptySem);
    CloseHandle(fullSem);
    CloseHandle(mutexSem);

    delete[] producerHandles;
    delete[] consumerHandles;
    delete[] producerIds;
    delete[] consumerIds;

    return 0;
}