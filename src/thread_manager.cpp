#include <iostream>
#include <unistd.h>      
#include <sys/wait.h>    
#include <signal.h>      
#include <vector>        
#include <cstdlib>       

using namespace std;

vector<pid_t> activeProcesses; 

int main() {
    int choice;
    pid_t pidToTerminate;

    cout << "Process and Thread Manager\n";

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Create a New Process\n";
        cout << "2. Terminate a Process\n";
        cout << "3. List Active Processes\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cout.flush(); 
        cin >> choice;

        switch (choice) {
            case 1:
                createProcess();
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
                cout << "Exiting. Terminating all active processes...\n";
                for (pid_t pid : activeProcesses) {
                    kill(pid, SIGTERM); 
                }
                activeProcesses.clear();
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
