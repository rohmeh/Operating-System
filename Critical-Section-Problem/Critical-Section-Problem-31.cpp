#include <windows.h>
#include <iostream>
#include <vector>

#define BUFFER_SIZE 20
#define MAX_ITEMS 100

struct SharedData {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    int SUM;
    int produced_count;
    int consumed_count;
    int active_producers;
    int m; // Number of producers
};

HANDLE hMapFile;
SharedData* pData;
HANDLE emptySemaphore;
HANDLE fullSemaphore;
HANDLE mutex;

DWORD WINAPI Producer(LPVOID param) {
    int id = *(int*)param;
    
    for (int i = 1; i <= MAX_ITEMS; i++) {
        WaitForSingleObject(emptySemaphore, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        // Critical section: produce item
        pData->buffer[pData->in] = i;
        pData->in = (pData->in + 1) % BUFFER_SIZE;
        pData->produced_count++;
        std::cout << "Producer " << id << " produced: " << i << std::endl;

        ReleaseMutex(mutex);
        ReleaseSemaphore(fullSemaphore, 1, NULL);
    }

    // Mark this producer as done
    WaitForSingleObject(mutex, INFINITE);
    pData->active_producers--;
    ReleaseMutex(mutex);

    return 0;
}

DWORD WINAPI Consumer(LPVOID param) {
    int id = *(int*)param;
    bool shouldExit = false;
    
    while (true) {
        // First check if we should exit
        WaitForSingleObject(mutex, INFINITE);
        shouldExit = (pData->active_producers == 0) && 
                   (pData->consumed_count == pData->m * MAX_ITEMS);
        ReleaseMutex(mutex);

        if (shouldExit) break;

        // Try to get an item with timeout
        DWORD result = WaitForSingleObject(fullSemaphore, 100); // 100ms timeout
        if (result == WAIT_TIMEOUT) {
            continue; // Check exit condition again
        }

        WaitForSingleObject(mutex, INFINITE);

        // Check exit condition again in critical section
        shouldExit = (pData->active_producers == 0) && 
                   (pData->consumed_count == pData->m * MAX_ITEMS);
        if (shouldExit) {
            ReleaseMutex(mutex);
            ReleaseSemaphore(fullSemaphore, 1, NULL); // Release the semaphore we just acquired
            break;
        }

        // Critical section: consume item
        int item = pData->buffer[pData->out];
        pData->out = (pData->out + 1) % BUFFER_SIZE;
        pData->SUM += item;
        pData->consumed_count++;
        std::cout << "Consumer " << id << " consumed: " << item << std::endl;

        ReleaseMutex(mutex);
        ReleaseSemaphore(emptySemaphore, 1, NULL);
    }

    return 0;
}

int main() {
    int m, n;
    std::cout << "Enter number of producers (m): ";
    std::cin >> m;
    std::cout << "Enter number of consumers (n): ";
    std::cin >> n;

    // Create shared memory
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(SharedData),
        "ProducerConsumerSharedMemory");

    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
        return 1;
    }

    pData = (SharedData*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(SharedData));

    if (pData == NULL) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // Initialize shared data
    pData->in = 0;
    pData->out = 0;
    pData->SUM = 0;
    pData->produced_count = 0;
    pData->consumed_count = 0;
    pData->active_producers = m;
    pData->m = m;

    // Create semaphores
    emptySemaphore = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, "EmptySemaphore");
    fullSemaphore = CreateSemaphore(NULL, 0, BUFFER_SIZE, "FullSemaphore");
    mutex = CreateMutex(NULL, FALSE, "BufferMutex");

    if (emptySemaphore == NULL || fullSemaphore == NULL || mutex == NULL) {
        std::cerr << "Failed to create semaphores/mutex" << std::endl;
        return 1;
    }

    // Create producer threads
    std::vector<HANDLE> producerThreads(m);
    std::vector<int> producerIds(m);
    for (int i = 0; i < m; i++) {
        producerIds[i] = i + 1;
        producerThreads[i] = CreateThread(
            NULL,
            0,
            Producer,
            &producerIds[i],
            0,
            NULL);
    }

    // Create consumer threads
    std::vector<HANDLE> consumerThreads(n);
    std::vector<int> consumerIds(n);
    for (int i = 0; i < n; i++) {
        consumerIds[i] = i + 1;
        consumerThreads[i] = CreateThread(
            NULL,
            0,
            Consumer,
            &consumerIds[i],
            0,
            NULL);
    }

    // Wait for all producers to finish
    WaitForMultipleObjects(m, producerThreads.data(), TRUE, INFINITE);
    
    // Wait for all consumers to finish
    WaitForMultipleObjects(n, consumerThreads.data(), TRUE, INFINITE);

    // Print final result
    std::cout << "Final SUM: " << pData->SUM << std::endl;
    std::cout << "Expected SUM: " << (m * MAX_ITEMS * (MAX_ITEMS + 1)) / 2 << std::endl;

    // Clean up
    for (auto& handle : producerThreads) CloseHandle(handle);
    for (auto& handle : consumerThreads) CloseHandle(handle);
    
    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);
    CloseHandle(emptySemaphore);
    CloseHandle(fullSemaphore);
    CloseHandle(mutex);

    return 0;
}