#include <windows.h>
#include <iostream>

struct ThreadData {
    int num1;
    int num2;
    int sum; 
};

DWORD WINAPI threadFunction(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    data->sum = data->num1 + data->num2; 
    return 0;
}

int main() {
    HANDLE threadHandle;
    DWORD threadID;
    ThreadData data;

    // Input two numbers
    std::cout << "Enter two numbers: ";
    std::cin >> data.num1 >> data.num2;

    // Create the thread
    threadHandle = CreateThread(
        NULL,                   // Default security attributes
        0,                      // Default stack size
        threadFunction,         // Thread function
        &data,                  // Data to pass to the thread
        0,                      // Default creation flags
        &threadID               // Thread ID
    );

    if (threadHandle == NULL) {
        std::cerr << "Failed to create thread!" << std::endl;
        return 1;
    }

    // Wait for the thread to finish
    WaitForSingleObject(threadHandle, INFINITE);

    CloseHandle(threadHandle);
    std::cout << "Sum: " << data.sum << std::endl;

    return 0;
}