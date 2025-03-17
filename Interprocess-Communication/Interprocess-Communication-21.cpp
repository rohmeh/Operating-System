#include<stdio.h>
#include<windows.h>
#include<time.h>
#define SHM_SIZE 1024
#define NUM_COUNT 100

int main() {
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHM_SIZE, "Local\\MySharedMemory");
    if (hMapFile == NULL) {
        printf("Could not create file mapping object(%lu).\n", GetLastError());
        return 1;
    }

    int *data = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);
    if (data == NULL) {
        printf("Could not map view of file(%lu).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < NUM_COUNT; i++) {
        data[i] = rand() % 100;
    }

    printf("Producer: Generated 100 random numbers and stored them in shared memory.\n");
    printf("Press Enter to exit the producer...\n");
    getchar();

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    return 0;
}