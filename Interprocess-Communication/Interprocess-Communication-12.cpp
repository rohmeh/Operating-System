#include<stdio.h>
#include<windows.h>
#define SHM_SIZE 1024

typedef struct {
    int number;
    char text[100];
} SharedData;

int main() {
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "Local\\MySharedMemory");
    if (hMapFile == NULL) {
        printf("Could not open file mapping object(%lu).\n", GetLastError());
        return 1;
    }

    SharedData *data = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);
    if (data == NULL) {
        printf("Could not map view of file(%lu).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    printf("Data read from shared memory: Number=%d, Text=%s\n", data->number, data->text);
    if (data->number % 2 == 0)
        printf("The number %d is even.\n", data->number);
    else
        printf("The number %d is odd.\n", data->number);

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    return 0;
}