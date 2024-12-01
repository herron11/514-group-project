#include <iostream>
#include <unistd.h>      
#include <sys/wait.h>    
#include <signal.h>      
#include <vector>        
#include <cstdlib>       
#include <thread>        
#include <mutex>         
#include <algorithm> 

using namespace std;

vector<pid_t> activeProcesses;
vector<thread> activeThreads;
mutex threadMutex;  

vector<vector<int>> allocation; 
vector<vector<int>> request;    
vector<int> available; 

void initializeResources(int resourceCount, int processCount) {
    allocation = vector<vector<int>>(processCount, vector<int>(resourceCount, 0));
    request = vector<vector<int>>(processCount, vector<int>(resourceCount, 0));
    available = vector<int>(resourceCount, 3);
}

void createThread() {
    lock_guard<mutex> lock(threadMutex);
    int threadId = activeThreads.size() + 1;
    activeThreads.emplace_back([threadId]() {
        cout << "Thread " << threadId << " is running.\n";
        cout.flush(); 
        this_thread::sleep_for(chrono::seconds(1)); //simulate process work
        cout << "Thread " << threadId << " has completed.\n";
        cout.flush(); 
    });

    if (activeThreads.back().joinable()) {
        activeThreads.back().join();
    }
}

void listThreads() {
    lock_guard<mutex> lock(threadMutex);
    if (activeThreads.empty()) {
        cout << "No active threads.\n";
    } else {
        cout << "Active Threads:\n";
        for (size_t i = 0; i < activeThreads.size(); ++i) {
            cout << " - Thread ID: " << i + 1 << "\n";
        }
    }
}

void joinThreads() {
    lock_guard<mutex> lock(threadMutex); 
    for (thread &t : activeThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    activeThreads.clear();
}

void createProcess(int resourceCount) {
    pid_t pid = fork();
    if (pid < 0) {
        cerr << "Failed to fork a process!\n";
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        cout << "Child Process: PID " << getpid() << ", Parent PID " << getppid() << "\n";
        cout.flush(); 
        while (true) {
            sleep(1); //needed so the child and parent process print before the menu prints again
        }
    } else {
        usleep(50000); //needed so the child and parent process print before the menu prints again (50ms delay)
        cout << "Parent Process: Created Child with PID " << pid << "\n";
        activeProcesses.push_back(pid);
        cout.flush();
    }
}
void terminateProcess(pid_t pid) {
    auto it = find(activeProcesses.begin(), activeProcesses.end(), pid);
    if (it != activeProcesses.end()) {
        if (kill(pid, SIGTERM) == 0) { 
            cout << "Terminated Process with PID " << pid << "\n";
            activeProcesses.erase(it); 
        } else {
            cerr << "Failed to terminate Process with PID " << pid << "\n";
        }
    } else {
        cerr << "Process with PID " << pid << " not active!\n";
    }
}
void listProcesses() {
    if (activeProcesses.empty()) {
        cout << "No active processes.\n";
    } else {
        cout << "Active Processes:\n";
        for (pid_t pid : activeProcesses) {
            cout << " - PID: " << pid << "\n";
        }
    }
}

bool detectDeadlock() {
    int processCount = allocation.size();
    int resourceCount = available.size();
    vector<bool> finish(processCount, false);
    vector<int> work = available;

    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < processCount; ++i) {
            if (!finish[i]) {
                bool canAllocate = true;
                for (int j = 0; j < resourceCount; ++j) {
                    if (request[i][j] > work[j]) {
                        canAllocate = false;
                        break;
                    }
                }
                if (canAllocate) {
                    for (int j = 0; j < resourceCount; ++j) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    progress = true;
                }
            }
        }
    }

    for (int i = 0; i < processCount; ++i) {
        if (!finish[i]) {
            cout << "Deadlock detected! Process " << activeProcesses[i] << " is in a deadlock state.\n";
            return true;
        }
    }

    cout << "No deadlock detected.\n";
    return false;
}
int main() {
    int choice;
    pid_t pidToTerminate;
    int resourceCount = 3; // Example: 3 resource types

    initializeResources(resourceCount, 0);
    cout << "Process and Thread Manager with Deadlock Detection\n";

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Create a New Process\n";
        cout << "2. Terminate a Process\n";
        cout << "3. List Active Processes\n";
        cout << "4. Detect Deadlock\n";
        cout << "5. List Active Threads\n";
        cout << "6. Detect Deadlock\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createProcess(resourceCount);
                break;
            case 2:
                createThread();
                break;
            case 3:
                cout << "Enter PID to terminate: ";
                cin >> pidToTerminate;
                terminateProcess(pidToTerminate);
                break;
            case 4:
                listProcesses();
                break;
            case 5:
                listThreads();
                break;
            case 6:
                detectDeadlock();
                break;
            case 7:
                cout << "Exiting. Terminating all active processes and joining all threads...\n";
                for (pid_t pid : activeProcesses) {
                    kill(pid, SIGTERM);
                }
                activeProcesses.clear();
                joinThreads();
                return 0;


            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
