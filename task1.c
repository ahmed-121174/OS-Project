#include <stdio.h>   // Standard I/O library for input and output operations
#include <stdlib.h>  // Standard library for functions like malloc, exit, etc.
#include <string.h>  // Library for string manipulation functions
#include <ctype.h>   // Library for character handling functions

// Define the structure for a task
typedef struct
{
    char name[10];   // Name of the task (max 9 characters + null terminator)
    int priority;    // Priority of the task (higher value = higher priority)
    int cpu_burst;   // CPU burst time required for the task
} task_t;

// Function prototypes for scheduling algorithms
void fcfs(task_t tasks[], int task_count);
void sjf(task_t tasks[], int task_count);
void priority_scheduling(task_t tasks[], int task_count);
void round_robin(task_t tasks[], int task_count, int time_quantum);
int load_tasks_from_file(const char* filename, task_t tasks[], int* task_count);

int main()
{
    task_t tasks[100]; // Array to store up to 100 tasks
    int task_count = 0; // Number of tasks initialized to 0

    // Load task list from the file schedule.txt
    if (!load_tasks_from_file("schedule.txt", tasks, &task_count))
    {
        printf("Error: Could not load tasks. Exiting program.\n");
        return 1; // Exit the program if task loading fails
    }

    // Call each scheduling algorithm and display the results
    printf("First-Come-First-Served (FCFS) Scheduling:\n");
    fcfs(tasks, task_count);

    printf("\nShortest Job First (SJF) Scheduling:\n");
    sjf(tasks, task_count);

    printf("\nPriority Scheduling:\n");
    priority_scheduling(tasks, task_count);

    printf("\nRound Robin Scheduling:\n");
    int time_quantum = 5; // Time quantum for Round Robin scheduling
    round_robin(tasks, task_count, time_quantum);

    return 0; // Exit the program successfully
}

// Function to load tasks from a file with error handling
int load_tasks_from_file(const char* filename, task_t tasks[], int* task_count)
{
    FILE* file = fopen(filename, "r"); // Open the file in read mode
    if (file == NULL)
    {
        printf("Error: Unable to open file %s.\n", filename);
        return 0; // Return 0 to indicate failure
    }

    int line = 0; // Line number tracker for error reporting
    while (fscanf(file, "%s %d %d", tasks[*task_count].name, &tasks[*task_count].priority, &tasks[*task_count].cpu_burst) != EOF)
    {
        line++; // Increment line number for each read
        // Validate task name length
        if (strlen(tasks[*task_count].name) > 9)
        {
            printf("Error: Task name too long on line %d. Skipping this task.\n", line);
            continue; // Skip this task and move to the next line
        }

        // Validate priority and CPU burst
        if (tasks[*task_count].priority <= 0 || tasks[*task_count].cpu_burst <= 0)
        {
            printf("Error: Invalid priority or CPU burst time on line %d. Skipping this task.\n", line);
            continue; // Skip this invalid task
        }

        (*task_count)++; // Increment the count of valid tasks
        if (*task_count > 100)
        {
            printf("Warning: Task limit exceeded. Only the first 100 tasks will be processed.\n");
            break; // Stop reading more tasks
        }
    }

    fclose(file); // Close the file

    if (*task_count == 0)
    {
        printf("Error: No valid tasks found in the file.\n");
        return 0; // Return 0 to indicate failure
    }

    return 1; // Return 1 to indicate successful loading of tasks
}

// FCFS Scheduling Algorithm
void fcfs(task_t tasks[], int task_count)
{
    int waiting_time = 0, turn_around_time = 0; // Initialize waiting and turnaround times

    printf("Task\tCPU Burst\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < task_count; i++)
    {
        turn_around_time += tasks[i].cpu_burst; // Increment turnaround time by current task's CPU burst
        printf("%s\t%d\t\t%d\t\t%d\n", tasks[i].name, tasks[i].cpu_burst, waiting_time, turn_around_time);
        waiting_time += tasks[i].cpu_burst; // Update waiting time for the next task
    }
}

// SJF Scheduling Algorithm (Non-preemptive)
void sjf(task_t tasks[], int task_count)
{
    // Sort tasks by CPU burst time (ascending order)
    for (int i = 0; i < task_count - 1; i++)
    {
        for (int j = i + 1; j < task_count; j++)
        {
            if (tasks[i].cpu_burst > tasks[j].cpu_burst)
            {
                task_t temp = tasks[i]; // Swap tasks to sort them
                tasks[i] = tasks[j];
                tasks[j] = temp;
            }
        }
    }

    int waiting_time = 0, turn_around_time = 0; // Initialize waiting and turnaround times
    printf("Task\tCPU Burst\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < task_count; i++)
    {
        turn_around_time += tasks[i].cpu_burst; // Increment turnaround time by CPU burst
        printf("%s\t%d\t\t%d\t\t%d\n", tasks[i].name, tasks[i].cpu_burst, waiting_time, turn_around_time);
        waiting_time += tasks[i].cpu_burst; // Update waiting time for the next task
    }
}

// Priority Scheduling Algorithm (Non-preemptive)
void priority_scheduling(task_t tasks[], int task_count)
{
    // Sort tasks by priority (higher priority first)
    for (int i = 0; i < task_count - 1; i++)
    {
        for (int j = i + 1; j < task_count; j++)
        {
            if (tasks[i].priority < tasks[j].priority)
            {
                task_t temp = tasks[i]; // Swap tasks to sort them
                tasks[i] = tasks[j];
                tasks[j] = temp;
            }
        }
    }

    int waiting_time = 0, turn_around_time = 0; // Initialize waiting and turnaround times
    printf("Task\tPriority\tCPU Burst\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < task_count; i++)
    {
        turn_around_time += tasks[i].cpu_burst; // Increment turnaround time by CPU burst
        printf("%s\t%d\t\t%d\t\t%d\t\t%d\n", tasks[i].name, tasks[i].priority, tasks[i].cpu_burst, waiting_time, turn_around_time);
        waiting_time += tasks[i].cpu_burst; // Update waiting time for the next task
    }
}

// Round Robin Scheduling Algorithm
void round_robin(task_t tasks[], int task_count, int time_quantum)
{
    int remaining_burst[100]; // Array to track remaining burst times
    for (int i = 0; i < task_count; i++)
    {
        remaining_burst[i] = tasks[i].cpu_burst;
    }

    int time = 0; // Initialize elapsed time
    printf("Task\tCPU Burst\tRemaining Time\n");
    while (1) // Loop until all tasks are completed
    {
        int done = 1; // Flag to check if all tasks are done
        for (int i = 0; i < task_count; i++)
        {
            if (remaining_burst[i] > 0)
            {
                done = 0; // Mark that work is pending
                if (remaining_burst[i] > time_quantum)
                {
                    remaining_burst[i] -= time_quantum; // Decrease remaining burst by quantum
                    time += time_quantum; // Increment elapsed time
                    printf("%s\t%d\t\t%d\n", tasks[i].name, tasks[i].cpu_burst, remaining_burst[i]);
                }
                else
                {
                    time += remaining_burst[i]; // Increment elapsed time
                    remaining_burst[i] = 0; // Mark the task as completed
                    printf("%s\t%d\t\t%d\n", tasks[i].name, tasks[i].cpu_burst, remaining_burst[i]);
                }
            }
        }
        if (done) break; // Exit loop if all tasks are completed
    }
}
