#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

struct Processes {
    string id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int finishTime = 0;
    int waitingTime = 0;
    int turnaroundTime = 0;
};

void calculateTime(vector<Processes>& processes) {
    for (size_t i = 0; i < processes.size(); i++) {
        processes[i].turnaroundTime = processes[i].finishTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
    }
}

void printResults(const vector<Processes>& processes, const string& ganttChart, float cpuUtilization) {
    cout << "\n--- Results ---\n";
    cout << "Gantt Chart: " << ganttChart <<  endl;

    cout << "Process\tFinish Time\tWaiting Time\tTurnaround Time\n";
    for (size_t i = 0; i < processes.size(); i++) {
        cout << processes[i].id << "\t \t"
            << processes[i].finishTime << "\t \t"
            << processes[i].waitingTime << "\t \t"
            << processes[i].turnaroundTime << endl;
    }
    cout << "CPU Utilization: " << fixed << setprecision(2) << cpuUtilization << "%\n";
}

void simulateFCFS(vector<Processes> processes) {
    cout << "\nSimulating FCFS...\n";
    int currentTime = 0;
    string ganttChart;
    float activeTime = 0;

    for (size_t i = 0; i < processes.size(); i++) {
        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime; 
        }

        for (int j = 0; j < processes[i].burstTime; j++) {
            ganttChart += processes[i].id[0];
        }
        ganttChart += " "; 

        currentTime += processes[i].burstTime;
        activeTime += processes[i].burstTime;

        processes[i].finishTime = currentTime;
    }

    calculateTime(processes);
    float cpuUtilization = (activeTime / currentTime) * 100;
    printResults(processes, ganttChart, cpuUtilization);
}

void simulateSRTF(vector<Processes> processes) {
    cout << "\nSimulating SRTF...\n";

    int currentTime = 0, activeTime = 0;
    string ganttChart;

    sort(processes.begin(), processes.end(),
        [](const Processes& a, const Processes& b) { return a.arrivalTime < b.arrivalTime; });

    auto compare = [](const Processes* a, const Processes* b) {
        if (a->remainingTime == b->remainingTime)
            return a->arrivalTime > b->arrivalTime;  
        return a->remainingTime > b->remainingTime;
        };
    priority_queue<Processes*, vector<Processes*>, decltype(compare)> readyQueue(compare);

    int index = 0;  
    while (!readyQueue.empty() || index < processes.size()) {
        while (index < processes.size() && processes[index].arrivalTime <= currentTime) {
            readyQueue.push(&processes[index]);
            index++;
        }

        if (readyQueue.empty()) {
            ganttChart += "_";  
            currentTime++;
            continue;
        }

       
        Processes* currentProcess = readyQueue.top();
        readyQueue.pop();

        ganttChart += currentProcess->id[0];
        currentProcess->remainingTime--;
        activeTime++;

      
        if (currentProcess->remainingTime == 0) {
            currentProcess->finishTime = currentTime + 1;
        }
        else {
            readyQueue.push(currentProcess);
        }

        currentTime++;
    }

    calculateTime(processes);
    float cpuUtilization = (activeTime / currentTime) * 100;
    printResults(processes, ganttChart, cpuUtilization);
}

void simulateRoundRobin(vector<Processes> processes, int quantum) {
    cout << "\nSimulating Round Robin...\n";
    int currentTime = 0;
    string ganttChart;
    float activeTime = 0;
    queue<Processes*> readyQueue;

  
    sort(processes.begin(), processes.end(),
        [](const Processes& a, const Processes& b) { return a.arrivalTime < b.arrivalTime; });

    int index = 0;  
    while (!readyQueue.empty() || index < processes.size()) {
       
        while (index < processes.size() && processes[index].arrivalTime <= currentTime) {
            processes[index].remainingTime = processes[index].burstTime; 
            readyQueue.push(&processes[index]);
            index++;
        }

        if (readyQueue.empty()) {
            ganttChart += "_"; 
            currentTime++;
            continue;
        }

        Processes* currentProcess = readyQueue.front();
        readyQueue.pop();

        int executionTime = min(quantum, currentProcess->remainingTime);
        ganttChart += string(executionTime, currentProcess->id[0]);
        currentTime += executionTime;
        activeTime += executionTime;
        currentProcess->remainingTime -= executionTime;

        if (currentProcess->remainingTime > 0) {
            readyQueue.push(currentProcess);
        }
        else {
            currentProcess->finishTime = currentTime;
        }
    }

    calculateTime(processes);
    float cpuUtilization = (activeTime / currentTime) * 100;
    printResults(processes, ganttChart, cpuUtilization);
}

int main() {
    vector<Processes> processes;
    int quantum;

    ifstream inputFile("Process.txt");
    if (!inputFile) {
        cout << "Error: Could not open input file." << endl;
        return 1;
    }

    string line;
    if (getline(inputFile, line)) {
        stringstream ss(line);
        ss >> quantum;
    }
    else {
       cout << "Error: Input file is empty or invalid." << endl;
        return 1;
    }

    while (getline(inputFile, line)) {
        stringstream ss(line);
        Processes p;
        ss >> p.id >> p.arrivalTime >> p.burstTime;
        p.remainingTime = p.burstTime; 
        processes.push_back(p);
    }
    inputFile.close();

    simulateFCFS(processes);
    simulateSRTF(processes);
    simulateRoundRobin(processes, quantum);

    return 0;
}
