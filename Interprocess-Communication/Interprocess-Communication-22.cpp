#include<stdio.h>
#include<windows.h>
#define SHM_SIZE 1024
#define NUM_COUNT 100

int main() {
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "Local\\MySharedMemory");
    if (hMapFile == NULL) {
        printf("Could not open file mapping object(%lu).\n", GetLastError());
        return 1;
    }

    int *data = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);
    if (data == NULL) {
        printf("Could not map view of file(%lu).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    int sum = 0;
    for (int i = 0; i < NUM_COUNT; i++) {
        sum += data[i];
    }

    printf("Consumer: The cumulative sum of the 100 numbers is %d.\n", sum);

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    return 0;
}