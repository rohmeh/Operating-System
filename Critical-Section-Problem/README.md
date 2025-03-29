# Operating System - Critical Section Problem

This repository contains solutions to three problems related to the **Critical Section Problem** in operating systems. These problems involve simulating race conditions, resolving them using semaphores, and solving the **m-producer n-consumer problem** with proper synchronization.

---

## Table of Contents
1. Problem 1: Simulating Race Conditions
2. Problem 2: Resolving Race Conditions with Semaphores
3. Problem 3: m-Producer n-Consumer Problem
4. How to Run the Code
5. Dependencies

---

## Problem 1: Simulating Race Conditions

### Description
In this problem, we simulate a **race condition** between two child processes (`C1` and `C2`) interacting with a shared variable `X`. Initially, `X = 0`. Process `C1` increments `X` by 2 ten times, while `C2` decrements `X` by 2 ten times. Without synchronization, the final value of `X` may deviate from 0 due to interleaving.

### Expected Behavior
- If there is no race condition, the final value of `X` should be `0`.
- With race conditions, the program sometimes produces non-zero values for `X`.

### File
- `Critical-Section-Problem-11.cpp`

---

## Problem 2: Resolving Race Conditions with Semaphores

### Description
This problem resolves the race condition from **Problem 1** by introducing **semaphores** to synchronize access to the shared variable `X`. Using synchronization mechanisms ensures that only one process (`C1` or `C2`) modifies `X` at a time.

### Expected Behavior
- The final value of `X` is always `0`, regardless of interleaving.
- No race conditions occur, ensuring consistent behavior across runs.

### File
- `Critical-Section-Problem-21.cpp`

---

## Problem 3: m-Producer n-Consumer Problem

### Description
This problem implements the **m-producer n-consumer problem** using a shared circular buffer of size 20. Each producer generates numbers from 1 to 100 and places them into the buffer, while consumers read these numbers and add them to a shared variable `SUM`. Proper synchronization ensures:
- Producers do not write when the buffer is full.
- Consumers do not read when the buffer is empty.
- Every number produced is consumed exactly once.

### Expected Behavior
- The final value of `SUM` equals the sum of all numbers produced by `m` producers:
  $$
  \text{SUM} = m \times \frac{100 \times (100 + 1)}{2} = m \times 5050
  $$
- For example, with `m = 4` and `n = 3`, the expected `SUM` is:
  $$
  \text{SUM} = 4 \times 5050 = 20200
  $$

### File
- `Critical-Section-Problem-31.cpp`

---

## How to Run the Code

### Prerequisites
- A C++ compiler (e.g., `g++`).
- Windows OS (the code uses Windows API functions like `CreateSemaphore`, `CreateMutex`, etc.).

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/your-repo-link.git
   cd your-repo-directory
   ```

2. Compile the desired program:
   ```bash
   g++ Critical-Section-Problem-1.cpp -o Problem1.exe
   g++ Critical-Section-Problem-2.cpp -o Problem2.exe
   g++ Critical-Section-Problem-3.cpp -o Problem3.exe
   ```

3. Run the executable:
   ```bash
   ./Problem1.exe
   ./Problem2.exe
   ./Problem3.exe
   ```

4. Follow the on-screen prompts to input values (e.g., number of producers and consumers).

