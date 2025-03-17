#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#define SHARED_MEMORY_SIZE 1024
#define ARRAY_SIZE 10
#define SHARED_MEMORY_NAME "Global\\SharedMemoryExample"

void createConsumerProcess(int start, int count) {
    char command[100];
    snprintf(command, sizeof(command), "consumer.exe %d %d", start, count);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Parent: Failed to create consumer process for range[%d to %d].\n", start, start + count - 1);
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() {
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEMORY_SIZE, SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        printf("Parent: Could not create file mapping. Error: %d\n", GetLastError());
        return 1;
    }

    int *data = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
    if (data == NULL) {
        printf("Parent: Could not map view of file. Error: %d\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    int numbers[ARRAY_SIZE] = {5, 4, 3, 2, 7, 6, 8, 9, 10, 12};
    for (int i = 0; i < ARRAY_SIZE; i++)
        data[i] = numbers[i];

    printf("Parent: Stored numbers in shared memory.\n");

    createConsumerProcess(0, 4); // Consumer 1: First 4 numbers
    createConsumerProcess(4, 2); // Consumer 2: Next 2 numbers
    createConsumerProcess(6, 4); // Consumer 3: Last 4 numbers

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);

    printf("Parent: All consumer processes finished execution.\n");
    return 0;
}