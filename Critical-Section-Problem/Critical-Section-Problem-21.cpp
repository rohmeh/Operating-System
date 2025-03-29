#include <iostream>
#include <windows.h> // For Windows API
#include <process.h> // For _beginthreadex
#include <cstdlib>   // For exit()

// Shared variable X
volatile int X = 0; // Use 'volatile' to prevent compiler optimizations

// Semaphore handle
HANDLE hSemaphore;

// Function for Child Process C1 (Increment X by 2, 10 times)
void incrementX(void* arg) {
    for (int i = 0; i < 10; ++i) {
        // Wait for the semaphore
        WaitForSingleObject(hSemaphore, INFINITE);

        // Critical Section: Increment X
        int temp = X;
        Sleep(1);           // Introduce a short delay to simulate interleaving
        X = temp + 2;
        std::cout << "C1 incremented X to: " << X << std::endl;

        // Release the semaphore
        ReleaseSemaphore(hSemaphore, 1, NULL);
    }
    _endthread(); // End thread
}

// Function for Child Process C2 (Decrement X by 2, 10 times)
void decrementX(void* arg) {
    for (int i = 0; i < 10; ++i) {
        // Wait for the semaphore
        WaitForSingleObject(hSemaphore, INFINITE);

        // Critical Section: Decrement X
        int temp = X;
        Sleep(1);           // Introduce a short delay to simulate interleaving
        X = temp - 2;
        std::cout << "C2 decremented X to: " << X << std::endl;

        // Release the semaphore
        ReleaseSemaphore(hSemaphore, 1, NULL);
    }
    _endthread(); // End thread
}

int main() {
    // Initialize X to 0
    X = 0;

    // Create a binary semaphore (initial count = 1, maximum count = 1)
    hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
    if (hSemaphore == NULL) {
        std::cerr << "Failed to create semaphore" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create handles for threads
    HANDLE hThreadC1, hThreadC2;

    // Start Child Process C1
    hThreadC1 = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall*)(void*))incrementX, NULL, 0, NULL);
    if (hThreadC1 == NULL) {
        std::cerr << "Failed to create thread for C1" << std::endl;
        CloseHandle(hSemaphore); // Clean up semaphore
        exit(EXIT_FAILURE);
    }

    // Start Child Process C2
    hThreadC2 = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall*)(void*))decrementX, NULL, 0, NULL);
    if (hThreadC2 == NULL) {
        std::cerr << "Failed to create thread for C2" << std::endl;
        CloseHandle(hThreadC1); // Clean up C1 thread
        CloseHandle(hSemaphore); // Clean up semaphore
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish
    WaitForSingleObject(hThreadC1, INFINITE);
    WaitForSingleObject(hThreadC2, INFINITE);

    // Close thread handles
    CloseHandle(hThreadC1);
    CloseHandle(hThreadC2);

    // Close the semaphore handle
    CloseHandle(hSemaphore);

    // Print the final value of X
    std::cout << "Final value of X: " << X << std::endl;

    return 0;
}