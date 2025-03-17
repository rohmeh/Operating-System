#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#define SHARED_MEMORY_SIZE 1024
#define SHARED_MEMORY_NAME "Global\\SharedMemoryExample"

unsigned long long factorial(int num) {
    if (num == 0 || num == 1)
        return 1;
    unsigned long long result = 1;
    for (int i = 2; i <= num; i++)
        result *= i;
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: consumer.exe <start_index> <count>\n");
        return 1;
    }

    int start = atoi(argv[1]);
    int count = atoi(argv[2]);

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        printf("Consumer: Could not open file mapping. Error: %d\n", GetLastError());
        return 1;
    }

    int *data = (int*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, SHARED_MEMORY_SIZE);
    if (data == NULL) {
        printf("Consumer: Could not map view of file. Error: %d\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    printf("Consumer handling indexes[%d to %d]:\n", start, start + count - 1);
    for (int i = start; i < start + count; i++) {
        printf("Factorial of %d is %llu\n", data[i], factorial(data[i]));
    }

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    return 0;
}