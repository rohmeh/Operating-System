#include <windows.h>
#include <iostream>
#include <string>

// Global variables for synchronization
HANDLE mutex; // Mutex for thread synchronization

// Thread function for T1: Creates the file "data.txt"
DWORD WINAPI threadT1(LPVOID lpParam) {
    // Acquire the mutex to ensure exclusive access
    WaitForSingleObject(mutex, INFINITE);

    // Create the file "data.txt"
    HANDLE hFile = CreateFile(
        "data.txt",                // File name
        GENERIC_WRITE,             // Open for writing
        0,                         // No sharing (exclusive access)
        NULL,                      // Default security attributes
        CREATE_ALWAYS,             // Overwrite if file exists
        FILE_ATTRIBUTE_NORMAL,     // Normal file
        NULL                       // No template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD errorCode = GetLastError();
        std::cerr << "Error: Failed to create file! Error code: " << errorCode << std::endl;
        ReleaseMutex(mutex); // Ensure mutex is released even if creation fails
        return 1;
    }

    std::cout << "Thread T1: File 'data.txt' created successfully." << std::endl;

    // Close the file handle in T1
    CloseHandle(hFile);

    // Release the mutex so T2 can proceed
    ReleaseMutex(mutex);

    return 0;
}

// Thread function for T2: Writes content to "data.txt"
DWORD WINAPI threadT2(LPVOID lpParam) {
    // Wait for T1 to finish creating the file
    WaitForSingleObject(mutex, INFINITE);

    // Open the file "data.txt" for writing
    HANDLE hFile = CreateFile(
        "data.txt",                // File name
        GENERIC_WRITE,             // Open for writing
        0,                         // No sharing (exclusive access)
        NULL,                      // Default security attributes
        OPEN_EXISTING,             // Open existing file
        FILE_ATTRIBUTE_NORMAL,     // Normal file
        NULL                       // No template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD errorCode = GetLastError();
        std::cerr << "Error: Failed to open file! Error code: " << errorCode << std::endl;
        ReleaseMutex(mutex); // Ensure mutex is released even if handle is invalid
        return 1;
    }

    // Content to write to the file
    std::string content = "This is the content written by Thread T2.\n";
    DWORD bytesWritten;

    // Write the content to the file
    BOOL result = WriteFile(
        hFile,                          // File handle
        content.c_str(),                // Data to write
        content.size(),                 // Size of data
        &bytesWritten,                  // Number of bytes written
        NULL                            // No overlapped structure
    );

    if (!result) {
        DWORD errorCode = GetLastError();
        std::cerr << "Error: Failed to write to file! Error code: " << errorCode << std::endl;
        CloseHandle(hFile);
        ReleaseMutex(mutex); // Ensure mutex is released even if write fails
        return 1;
    }

    std::cout << "Thread T2: Content written to 'data.txt' successfully." << std::endl;

    // Close the file handle
    CloseHandle(hFile);

    // Release the mutex after completing the task
    ReleaseMutex(mutex);

    return 0;
}

int main() {
    HANDLE thread1, thread2;
    DWORD threadID1, threadID2;

    // Create a mutex for synchronization between threads
    mutex = CreateMutex(NULL, FALSE, NULL); // Initially not owned by any thread

    if (mutex == NULL) {
        DWORD errorCode = GetLastError();
        std::cerr << "Error: Failed to create mutex! Error code: " << errorCode << std::endl;
        return 1;
    }

    // Acquire the mutex initially in the main thread
    WaitForSingleObject(mutex, INFINITE);

    // Create Thread T1
    thread1 = CreateThread(
        NULL,                   // Default security attributes
        0,                      // Default stack size
        threadT1,               // Thread function
        NULL,                   // No parameters
        0,                      // Default creation flags
        &threadID1              // Thread ID
    );

    if (thread1 == NULL) {
        DWORD errorCode = GetLastError();
        std::cerr << "Error: Failed to create Thread T1! Error code: " << errorCode << std::endl;
        CloseHandle(mutex);
        return 1;
    }

    // Create Thread T2
    thread2 = CreateThread(
        NULL,                   // Default security attributes
        0,                      // Default stack size
        threadT2,               // Thread function
        NULL,                   // No parameters
        0,                      // Default creation flags
        &threadID2              // Thread ID
    );

    if (thread2 == NULL) {
        DWORD errorCode = GetLastError();
        std::cerr << "Error: Failed to create Thread T2! Error code: " << errorCode << std::endl;
        CloseHandle(thread1);
        CloseHandle(mutex);
        return 1;
    }

    // Release the mutex so T1 can proceed
    ReleaseMutex(mutex);

    // Wait for both threads to finish
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    // Clean up handles
    CloseHandle(thread1);
    CloseHandle(thread2);
    CloseHandle(mutex);

    std::cout << "Main: Both threads completed successfully." << std::endl;

    return 0;
}