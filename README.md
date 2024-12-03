# Process and Thread Manager

### Project Overview
This project implements a Process and Thread Manager that allows users to create and manage both processes and threads, tracks resource allocation to processes, and identifies potential deadlocks in the system.

### Features
- Process creation and termination
- Create/manage multiple processes and threads
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

After running the program, you will see a menu with the following options:

- Create a New Process: Forks a new child process and tracks its PID.

- Create a New Thread: Starts a new thread that simulates work.

- Terminate a Process: Enter the PID of the process you want to terminate.

- List Active Processes: Displays all active processes and their PIDs.

- List Active Threads: Displays all active threads.

- Detect Deadlock: Runs the deadlock detection algorithm to identify any processes in a deadlock state.

- Exit: Terminates all active processes and joins all threads before safely exiting the program.

Enter the number in the menu corresponding to the option you want to choose.

</br>

To remove the executable file after you exit the program, run:
```
make clean
```


