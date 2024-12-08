#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <atomic>
#include <map>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

// Shared memory data structure
struct SharedMemory {
    atomic<int> resourceAvailable;
    map<int, int> processPriorities; // Store process priorities
};

struct ThreadTask {
    int id;
    int priority;
    function<string()> task;

    bool operator<(const ThreadTask& other) const {
        return priority > other.priority;
    }
};

// Shared memory pointer
SharedMemory* sharedMemory;

vector<pid_t> activeProcesses;
vector<thread> activeThreads;
mutex threadMutex;

vector<vector<int>> allocation;
vector<vector<int>> request;
vector<int> available;

priority_queue<ThreadTask> threadPoolQueue;
vector<thread> threadPool;
mutex threadPoolMutex;
condition_variable threadPoolCV;
bool stopThreadPool = false;
int maxThreads = 5;
atomic<bool> taskRunning(false);

map<int, bool> taskCompletionStatus;
map<int, string> taskOutputs;
mutex coutMutex;

// Mutex with priority inheritance
mutex resourceMutex;

void initializeResources(int resourceCount) {
    allocation.clear();
    request.clear();
    available = vector<int>(resourceCount, 9); 
}

void createThread() {
    lock_guard<mutex> lock(threadMutex);
    int threadId = activeThreads.size() + 1;
    activeThreads.emplace_back([threadId]() {
        cout << "Thread " << threadId << " is running.\n";
        cout.flush();
        this_thread::sleep_for(chrono::seconds(1)); // Simulate thread work
        cout << "Thread " << threadId << " has completed.\n";
        cout.flush();
    });

    if (activeThreads.back().joinable()) {
        activeThreads.back().join();
    }
}

void listThreads() {
    lock_guard<mutex> lock(threadMutex);
    if (threadPool.empty()) {
        cout << "No active threads.\n";
    } else {
        cout << "Active Threads: " << threadPool.size() << " in pool\n";
        cout << "Created threads: " << activeThreads.size() << "\n";
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
            sleep(1); // Needed so the child and parent process print before the menu prints again
        }
    } else {
        usleep(50000); // Needed so the child and parent process print before the menu prints again (50ms delay)
        cout << "Parent Process: Created Child with PID " << pid << "\n";
        activeProcesses.push_back(pid);
        vector<int> newRequest(resourceCount);
        srand(time(0)); 
        for (int i = 0; i < resourceCount; ++i) {
            newRequest[i] = rand() % 6; 
        }

        bool canAllocate = true;
        for (int i = 0; i < resourceCount; ++i) {
            if (newRequest[i] > available[i]) {
                canAllocate = false;
                break;
            }
        }
        if (canAllocate) {
            for (int i = 0; i < resourceCount; ++i) {
                available[i] -= newRequest[i];
            }
            allocation.push_back(newRequest);
            request.push_back(vector<int>(resourceCount, 0));
            cout << "Process " << pid << " created and resources allocated.\n";
        } else {
            allocation.push_back(vector<int>(resourceCount, 0)); 
            request.push_back(newRequest);
            cout << "Process " << pid << " created but requested resources are unavailable.\n";
            cout << "Requested: ";
            for (int r : newRequest) {
                cout << r << " ";
            }
            cout << "\n";
        }
    }
}

void terminateProcess(pid_t pid) {
    auto it = find(activeProcesses.begin(), activeProcesses.end(), pid);
    if (it != activeProcesses.end()) {
        int index = distance(activeProcesses.begin(), it);

        for (int i = 0; i < available.size(); ++i) {
            available[i] += allocation[index][i];
        }

        allocation.erase(allocation.begin() + index);
        request.erase(request.begin() + index);
        activeProcesses.erase(it);

        if (kill(pid, SIGTERM) == 0) {
            cout << "Terminated Process with PID " << pid << "\n";
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

// Priority Inheritance Protocol Implementation
void acquireResource(int threadPriority) {
    lock_guard<mutex> lock(resourceMutex);
    int currentThreadPriority = threadPriority;
    if (sharedMemory->processPriorities.find(getpid()) != sharedMemory->processPriorities.end()) {
        // Inherit priority if current thread has lower priority than the thread requesting resource
        currentThreadPriority = max(currentThreadPriority, sharedMemory->processPriorities[getpid()]);
    }

    sharedMemory->processPriorities[getpid()] = currentThreadPriority;
    cout << "Thread " << getpid() << " acquired resource with priority: " << currentThreadPriority << "\n";
}

void releaseResource() {
    lock_guard<mutex> lock(resourceMutex);
    sharedMemory->processPriorities.erase(getpid());
    cout << "Thread " << getpid() << " released resource.\n";
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
    srand(time(0));
    int choice;
    pid_t pidToTerminate;
    int resourceCount = 3; // Example: 3 resource types

    // Initialize shared memory
    int shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedMemory));
    sharedMemory = (SharedMemory*)mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sharedMemory->resourceAvailable.store(9);

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Create Process\n";
        cout << "2. Terminate Process\n";
        cout << "3. List Processes\n";
        cout << "4. Detect Deadlock\n";
        cout << "5. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createProcess(resourceCount);
                break;
            case 2:
                cout << "Enter PID to terminate: ";
                cin >> pidToTerminate;
                terminateProcess(pidToTerminate);
                break;
            case 3:
                listProcesses();
                break;
            case 4:
                detectDeadlock();
                break;
            case 5:
                munmap(sharedMemory, sizeof(SharedMemory));
                shm_unlink("/my_shared_memory");
                exit(0);
            default:
                cout << "Invalid choice, try again.\n";
        }
    }

    return 0;
}
