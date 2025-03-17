#include <windows.h>
#include <iostream>
#include <vector>

// Global variables for thread cancellation and synchronization
volatile bool stopThreads = false; // Flag to signal threads to stop
HANDLE mutex;                      // Mutex for synchronizing console output

// Function executed by Thread T1 and T2
DWORD WINAPI threadFunction(LPVOID lpParam) {
    // Cast the input parameter to a vector of integers
    std::vector<int>* arr = static_cast<std::vector<int>*>(lpParam);

    for (size_t i = 0; i < arr->size(); ++i) {
        // Check if the thread should stop
        if (stopThreads) {
            WaitForSingleObject(mutex, INFINITE);
            std::cout << "Thread exiting due to cancellation." << std::endl;
            ReleaseMutex(mutex);
            return 0; // Exit the thread
        }

        // Process the array element: add 2 and multiply by 4
        (*arr)[i] = ((*arr)[i] + 2) * 4;

        // Print only every 50th element to reduce console clutter
        if (i % 50 == 0) {
            WaitForSingleObject(mutex, INFINITE);
            std::cout << "Processed element at index " << i << ": " << (*arr)[i] << std::endl;
            ReleaseMutex(mutex);
        }

        // Simulate minimal delay to mimic processing time
        Sleep(0); // No sleep or minimal sleep to speed up processing
    }

    return 0;
}

int main() {
    // Create two arrays of size 10,000 for T1 and T2
    const size_t ARRAY_SIZE = 10000;
    std::vector<int> array1(ARRAY_SIZE, 1); // Initialize all elements to 1
    std::vector<int> array2(ARRAY_SIZE, 1); // Initialize all elements to 1

    // Thread handles
    HANDLE thread1, thread2;

    // Create a mutex for synchronizing console output
    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        std::cerr << "Error: Failed to create mutex!" << std::endl;
        return 1;
    }

    // Create Thread T1
    thread1 = CreateThread(
        NULL,                   // Default security attributes
        0,                      // Default stack size
        threadFunction,         // Thread function
        &array1,                // Pass array1 as input
        0,                      // Default creation flags
        NULL                    // Thread ID (not needed)
    );

    if (thread1 == NULL) {
        std::cerr << "Error: Failed to create Thread T1!" << std::endl;
        CloseHandle(mutex);
        return 1;
    }

    // Create Thread T2
    thread2 = CreateThread(
        NULL,                   // Default security attributes
        0,                      // Default stack size
        threadFunction,         // Thread function
        &array2,                // Pass array2 as input
        0,                      // Default creation flags
        NULL                    // Thread ID (not needed)
    );

    if (thread2 == NULL) {
        std::cerr << "Error: Failed to create Thread T2!" << std::endl;
        CloseHandle(thread1);
        CloseHandle(mutex);
        return 1;
    }

    // Let the threads run for 5 seconds
    Sleep(5000); // Sleep for 5 seconds

    // Signal the threads to stop
    stopThreads = true;

    // Wait for both threads to finish
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    // Clean up handles
    CloseHandle(thread1);
    CloseHandle(thread2);
    CloseHandle(mutex);

    std::cout << "Main: Both threads canceled successfully." << std::endl;

    return 0;
}