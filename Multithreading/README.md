# Multithreading Assignment

This repository contains solutions for a multithreading assignment in the Operating Systems course. The assignment focuses on implementing multithreaded programs using Windows Threads in C++.

## Repository Contents

### Files Overview
- **`multithreading-01.cpp`**: Solution to the first question, where two threads are created: one creates a file, and the other writes content to it.
- **`multithreading-02.cpp`**: Solution to the second question, where two threads process arrays by applying transformations (`(x + 2) * 4`) and are canceled after 1 second.
- **`multithreading-03.cpp`**: Solution to the third question.
- **`Multithreading-Solutions.pdf`**: Detailed solutions and explanations for the assignment questions.
- **`Multithreading-Assignment.pdf`**: The original assignment questions and requirements.

---

### How to Use

1. **Compile the Code**:
   - Ensure you have a C++ compiler installed (e.g., `g++` for MinGW or `cl` for Microsoft Visual Studio).
   - Compile each `.cpp` file using the following command:
     ```bash
     g++ multithreading-01.cpp -o multithreading-01.exe -lkernel32
     ```
   - Run the executable:
     ```bash
     ./multithreading-01.exe
     ```

2. **Understand the Solutions**:
   - Refer to `Multithreading-Solutions.pdf` for detailed explanations of the implemented solutions.
   - Check `Multithreading-Assignment.pdf` for the original problem statements.

3. **Explore the Code**:
   - Each `.cpp` file corresponds to a specific question in the assignment. Comments in the code provide additional context.

---

### Notes

- The programs use **Windows Threads** (`CreateThread`, `WaitForSingleObject`, etc.) and are designed to run on Windows systems.
- For cross-platform compatibility, consider using `std::thread` from C++11 or higher.
- Ensure proper permissions for file operations (e.g., creating/writing to files).

---

### Contributions

Feel free to suggest improvements or report issues. All contributions are welcome!
