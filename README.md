# OS4: Multilevel Feedback Queue Simulator

This project simulates an operating system by launching and scheduling the runtimes of multiple child processes. It incorporates elements of the previous three OS projects, such as Process Control Block 
(PCB) tables, simulated system clocks, and message queues. 

To expand on those projects, this OS simulation introduces a **Multilevel Feedback Queue**. The queue's goal is to facilitate communication and scheduling among parent (OSS) and child (worker) processes. 
The operating system utilizes the feedback queue to schedule child processes, one at a time. When a child is scheduled, it will execute for a specific period of time known as a **time quantum**. Meanwhile, the operating 
system also keeps track of a process's:

  - **Dispatch times** - the length of time between the *previous* process's runtime ***ending point*** and the *current* process's runtime ***starting point***.
  - **Queue level** - the individual queue within the Multilevel Feedback queue. A process's time quantum depends on the queue level in which it resides.
  - **Blocked state** - the state of ability (or inability) to execute. If a child is blocked, then it cannot excecute. Therefore, the feedback queue must try to schedule the next child.
    
## Key Features:
- Uses shared memory to simulate a system clock that tracks time in seconds and nanoseconds.
- Uses a message queue to communicate between OSS and its children.
- Uses a **Multilevel Feedback Queue** to schedule the communication between parent and child which operates from the message queue.
- Implements:
  - `fork()` for process creation.
  - `msgsnd()` and `msgrcv()` for sending and receiving messsages within the message queue.
  - `waitpid()` for termination of child processes.
- Uses a Process Control Block (PCB) table to record metadata, such as:
  - Process ID (PID).
  - Table slot occupancy.
  - Process **start time**.
  - Process **service time** (for a process to run).
  - Process **event wait time** (for a process to become unblocked).
  - Indication of whether a child is **blocked** (or unable to run temporarily).
- Ensures clean termination of all processes to avoid zombie processes.

## System Clock Operations:
When the program begins and *no* processes are ready to launch yet, the **simulated system clock** increments at a fixed rate of **100 milliseconds**. This setup continues until the first child process can finally launch.

After the first process has entered the system, time becomes *added* into the system clock based on three scenarios:

> 1.) **Dispatch time** occurs between two child processes.
  
> 2.) A child utilizes its **time quantum** when running.
  
> 3.) ALL processes in the system reside in the **blocked state**.

For instance, Scenario #1 is used. The simulated system clock reads at **400,000,000 nanoseconds**. A child undergoes a dispatch time of **5,000 nanoseconds**. The dispatch time becomes added to the system 
clock time in this manner: 

$$
\text{sysClockTime} 
= \text{sysClockTime + dispatchTime}
$$

$$
\text{sysClockTime} 
= \text{400,000,000 ns + 5,000 ns}
= \textbf{400,005,000 ns}
$$

The new system clock time becomes **4,005,000 nanoseconds**. Direct additions into the system clock time occur throughout the program's duration.
## How to Compile and Run:

1.) To *compile* the program, type:
```bash
make
```
2.) To *run* the program, type:
```bash
./oss -f <logfile_name>
```
### Command-Line Options (user inputted):

These command-line options are for users to input into the console.

| Option | Argument Format              | Required (yes/no)     | Description                                                                                         |
|--------|------------------------------|-----------------------|-----------------------------------------------------------------------------------------------------|
|  `-h`  | *(none)*                     | No                    | Displays help menu, then terminates program.                                                        |                                
|  `-f`  | a string.                    | No (default="logfile")| Sets a basename for a .txt file, which stores logging information about child processes.            |

### Command-Line Options (legacy):

These command-line options were used in other OS projects, but cannot be inputted into the console in this project. Instead, these options were either hardcoded into the program's code or handled by using some other
method.

This table below may help the user understand how and when child processes launch and terminate, as well as how they can exist concurrently.

| Option | Description                                                                                         | Current Option Handling Method
|--------|-----------------------------------------------------------------------------------------------------|-----------------------------------------------------|
|  `-n`  | Sets a total number of processes to run.                                                            | Value hardcoded to 10 processes.                               
|  `-s`  | Sets a maximum number of processes to run simultaneously.                                           | Value hardcoded to 10 processes.
|  `-t`  | Sets a maximum number of seconds each process runs. Actual value is random between 1 and -t value.  | Value depends on when user.c decides the process should terminate.                           
|  `-i`  | Sets an interval between child process launches (in milliseconds).                                  | Value is randomized between 1 and 500 ms.

## Example Output:
### Example 1: Console and Log File Output
#### i.) Program Initialization:
#### ii.) Process Runtime:
#### iii.) Concurrency:
#### iv.) Process Termination:
#### v.) Program Termination:

### Example 2: Process Control Block (PCB) Table Output
Every half second of simulated system time, a Process Table prints to the console and log file as shown below:

```
OSS PID: 3951674  SysClockS: 2  SysClockNano: 160026175
Process Table:
Entry     Occupied     PID        StartS     StartN        ServiceS     ServiceN      EventWaitS    EventWaitN        Blocked
0         1            3951676    0          746930885     0            647600000     5             213833997         1
1         1            3951683    1          7394623       0            510000000     0             0                 0
2         1            3951702    1          505184876     0            270000000     0             0                 0
3         1            3951716    1          919458854     0            110000000     0             0                 0
4         0            0          0          0             0            0             0             0                 0
...
```
A full Process Table contains 20 rows, one for each child process slot. 

#### Column Definitions:
- **Entry** - the index of the process that resides in the table.
- **Occupied** - is the process running? (**1** if *yes*; **0** if *no*)
- **PID** - the Process ID of a child inside a table row.
- **StartS** - start time of the process in *seconds*.
- **StartN** - start time of the process in *nanoseconds*.
- **ServiceS** - total time that a process was scheduled in *seconds*.
- **ServiceN** - total time that a process was scheduled in *nanoseconds*.
- **EventWaitS** - time that the system clock must reach before a process becomes unblocked in *seconds*.
- **EventWaitN** - time that the system clock must reach before a process becomes unblocked in *nanoseconds*.
- **Blocked** - is the process blocked? (**1** if *yes*; **0** if *no*)
  
## Skills Learned:

## Tested On:
- Ubuntu 20.04.6 (LTS)
- GCC 10.5.0
- Make 4.2.1

## License:
This project is licensed under the [MIT License](LICENSE).
