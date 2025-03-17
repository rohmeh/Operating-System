#include<stdio.h>
#include<windows.h>
#define SHM_SIZE 1024

typedef struct {
    int number;
    char text[100];
} SharedData;

int main() {
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHM_SIZE, "Local\\MySharedMemory");
    if (hMapFile == NULL) {
        printf("Could not create file mapping object(%lu).\n", GetLastError());
        return 1;
    }

    SharedData *data = (SharedData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);
    if (data == NULL) {
        printf("Could not map view of file(%lu).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    printf("Enter an integer:");
    scanf("%d", &data->number);
    printf("Enter a string:");
    scanf("%s", data->text);

    printf("Data written to shared memory: Number=%d, Text=%s\n", data->number, data->text);
    printf("Press Enter to exit the producer...\n");
    getchar();
    getchar();

    UnmapViewOfFile(data);
    CloseHandle(hMapFile);
    return 0;
}