# Process and Thread Manager

### Project Overview
This project implements a Process and Thread Manager that allows users to create and manage processes and threads, tracks resource allocation to processes, create and manages a thread pool, and identifies potential deadlocks in the system.

### Features
- Create and manage multiple processes and threads
- Thread Pool Management
- Resource Allocation Tracking
- Deadlock Detection


### How To Run The Program

Clone the repository in your ROS2 workspace:
```
git clone https://github.com/herron11/514-group-project.git
```
Move into the 514-group-project/src folder:
```
cd 514-group-project/src
```

Build the makefile and run the program:
```
make
./thread_manager
```
To remove the executable file after you exit the program, run:
```
make clean
```

### Usage

To test the code, run the program and you will see a menu with the options below. Enter the number in the menu corresponding to the option you want to test.

1. Create a New Process: Forks a new child process, tracks its PID, and requests resources.
2. Terminate a Process: Terminate a process and releases its resources.
3. Create a New Thread: Starts a new thread that simulates work.
4. List Active Processes: Displays all active processes and their PIDs.
5. List Active Threads: Display the number of active threads in the pool and created by the user.
6. Display Resources: Prints available resources, allocation matrix, and request matrix.
7. Add Task to Thread Pool: Add a task to the thread pool by entering a task ID and priority level.
8. Detect Deadlock: Runs the deadlock detection algorithm to identify any processes in a deadlock state.
9. Exit: Terminates all active processes and joins all threads before safely exiting the program.

To test the resource allocation tracking:
- Press 6 to display the currently available resources. You should see 9 9 9.
- Press 1 to create a process.
- Press 6 again to see resources available after fulfilling the processes request.
- You can create more processes or press 2 to terminate the initial process you created.
- Press 6 again to confirm that the process has released its resources back into the available pool.


### Design Decisions
- For simplicity, threads and process sleep to simulate work.
- We allow the user to create threads separate from the pool, bypassing the pool overhead.
- We implemented a menu for accesibility and ease of use for the user.

### Known Limitations
- The threads spawned during process creation are not tracked in the thread counts.
- Resources used by the threads are not tracked.
- Process waiting for resources when they are unavailable do not get the resources when they become available.

### References
- https://www.geeksforgeeks.org/std-mutex-in-cpp/
- https://en.cppreference.com/w/cpp/thread/mutex
- https://man.freebsd.org/cgi/man.cgi?fork(2)
- https://man.freebsd.org/cgi/man.cgi?query=getpid&sektion=2
- https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
- https://en.cppreference.com/w/cpp/types/size_t
- https://youtu.be/-Qa1RqmXKG0

### Authors
- Andre' Herron
- Matthew Akinmolayan
- Tobiloba Ayodeji
- Tobiloba Ifenikalo
