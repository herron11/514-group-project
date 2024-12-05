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

### Usage

After running the program, you will see a menu with the following options. Enter the number in the menu corresponding to the option you want to choose.

- Create a New Process: Forks a new child process and tracks its PID.

- Create a New Thread: Starts a new thread that simulates work.

- Terminate a Process: Enter the PID of the process you want to terminate.

- List Active Processes: Displays all active processes and their PIDs.

- List Active Threads: Display the number of active threads in the pool and created by the user.

- Detect Deadlock: Runs the deadlock detection algorithm to identify any processes in a deadlock state.
  
- Display Resources: Prints the resource allocation matrix, request matrix, and available resources.

- Add Task to Thread Pool: Add a task to the thread pool by entering a task ID and priority level. 

- Exit: Terminates all active processes and joins all threads before safely exiting the program.



</br>

To remove the executable file after you exit the program, run:
```
make clean
```
### Design Decisions
- For simplicity, threads and process sleep to simulate work.
- We allow the user to create threads separate from the pool, bypassing the pool overhead.
- We implemented a menu for accesibility and ease of use for the user.

### Known Limitations
- The threads spawned during process creation are not tracked in the thread counts.

### References
- https://www.geeksforgeeks.org/std-mutex-in-cpp/
- https://en.cppreference.com/w/cpp/thread/mutex
- https://man.freebsd.org/cgi/man.cgi?fork(2)
- https://man.freebsd.org/cgi/man.cgi?query=getpid&sektion=2
- https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
