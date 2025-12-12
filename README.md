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

## Program Code Operations:
### Simulated System Clock:
When the program begins and *no* processes are ready to launch yet, the **simulated system clock** increments at a fixed rate of **100 milliseconds**. This setup continues until the first child process can finally launch.

After the first process has entered the system, time becomes *added* into the system clock based on three scenarios:
  1. **Dispatch time** occurs between two child processes.
  2. A child utilizes its **time quantum** when running.
  3. ALL processes in the system reside in the **blocked state**.

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

### Message Queue:
The two files, *oss.c* and *user.c*, both utilize two different message queues, `sendBuffer` and `receiveBuffer`. They both operate under a struct that contains two members:
  - `messageType`- stores the child process's **Process ID (PID)**.
    - Both *oss.c* and *user.c* files use this information to determine which child process must receive a message and send it back.
  - `quantumData`- stores a runtime value in nanoseconds. However, the two files use these numbers slightly differently:
    - **oss.c** (i.e., the parent) stores the *expected runtime* known as the **time quantum**. OSS sends this value to user.c so a child process can decide whether to            fully or partially run the time quantum.
    - **user.c** (i.e. the child) stores the **actual runtime**. This value is the amount of time for which a child process *actually* runs. Keep in mind that a process
      may NOT utilize its entire time quantum.

### Multilevel Feedback Queue:
For this project, the Multilevel Feedback Queue (MLFQ) serves as a scheduling mechanism that OSS uses to determine which child process should run next. The feedback queue operates by working with three priority queue levels, each with its own time quantum:

<div align="center">
  
| Queue Level         | Enumeration | Time Quantum           |
|---------------------|-------------|------------------------|  
|**High-priority**    | 0           | 10,000,000 nanoseconds |     
|**Medium-priority**  | 1           | 20,000,000 nanoseconds |     
|**Low-priority**     | 2           | 40,000,000 nanoseconds |    
|**Blocked**          | 3           | (does not apply)       |   

</div>

When a new process enters the system, the Process ID always begins in the back of the **high-priority** queue. If the new process runs and utilizes its entire time quantum of 10,000,000 nanoseconds, it will become relocated from the high-priority queue to the **medium-priority** queue. Once the process runs in the medium-priority queue for 20,000,000 nanoseconds, it will transfer to the **low-priority** queue. As the child now resides inside the low-priority queue, it will perform one of three tasks: 

1.) Run for its entire 40,000,000 time quantum. 

2.) Undergo an **I/O interrupt** and become sent to the *blocked queue*. Once it becomes *unblocked*, it will be placed back into the high-priority queue to run again sooner.
  
  - A process becomes *unblocked* when the system clock time runs past the process's **event wait time**. This time period can be referenced in the Process Control Block (PCB) table.
  
3.) **Terminate** so that OSS removes it from the system.


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
|  `-t`  | Sets a maximum number of seconds each process runs. Actual value is random between 1 and -t value.  | Value depends on when user.c decides the process should                                                                                                                      terminate. user.c does not make the decision based                                                                                                                           on the number of seconds. Instead, children terminate                                                                                                                        based on a ***probability function*** in user.c. Refer to                                                                                                                   this README's "Feedback Queue Operations" section above for                                                                                                                  more information.
|  `-i`  | Sets an interval between child process launches (in milliseconds).                                  | Value is randomized between 1 and 500 ms.

## Example Output:
### Example 1: Console and Log File Output
#### i.) Program Initialization:
The program initializes by using **fork()** to launch a process with PID **916494**, whihch is launched after `304,289,380` nanoseconds of system runtime. The PID becomes placed inside of Queue 0 (the **high-priority** queue) as shown below:

```bash
++OSS: Generating process with PID 916494 and putting it in queue 0 at time 0:304289380
     Queue 0: 916494
     Queue 1: (empty)
     Queue 2: (empty)
     Queue 3: (empty)
```

#### ii.) Process Runtime 
Before each process runtime, a message below confirms that the feedback queue is dispatching the process (in this case, PID **919633**). The output also reveals the dispatch time of `9,944` nanoseconds, which then becomes added to the previous system clock time of `304,289,380` nanoseconds.

```bash
OSS: Dispatching process with PID 919633 from queue 0 at time 0:304299324
OSS: Total time spent in dispatch was 9944 nanoseconds
```

The "parent" (i.e., OSS) then makes a call to **msgsnd()** to send a message to the child process containing a PID and the expected time quantum of `10,000,000` nanoseconds. In this situation, the child (i.e., the worker) decides that it should run for the entire `10,000,000` nanoseconds. PID **919633** then becomes sent into Queue 1 (the **medium-priority** queue).

```bash
OSS: Receiving that process with PID 919633 ran for 10000000 nanoseconds
     Queue 0: (empty)
     Queue 1: 919633
     Queue 2: (empty)
     Queue 3: (empty)
```

The same situation occurs again for PID **919633**, except it runs for `20,000,000` nanoseconds in Queue 1, then becomes moved into Queue 2 (the **low-priority** queue).

```bash
OSS: Dispatching process with PID 919633 from queue 1 at time 0:314300745
OSS: Total time spent in dispatch was 1421 nanoseconds
OSS: Receiving that process with PID 919633 ran for 20000000 nanoseconds
     Queue 0: (empty)
     Queue 1: (empty)
     Queue 2: 919633
     Queue 3: (empty)
```

Since PID **919633** is already in the low-priority queue, it will stay there and attempt to run the entire `40,000,000` nanosecond time quantum. It will continue to do so until the worker decides that the child should be blocked or terminated. This is expressed by the repetitive output shown below:

 ```bash
OSS: Dispatching process with PID 919633 from queue 2 at time 0:334305035
OSS: Total time spent in dispatch was 4290 nanoseconds
OSS: Receiving that process with PID 919633 ran for 40000000 nanoseconds
OSS: Dispatching process with PID 919633 from queue 2 at time 0:374309238
OSS: Total time spent in dispatch was 4203 nanoseconds
OSS: Receiving that process with PID 919633 ran for 40000000 nanoseconds
```

#### iii.) Process Blocking:
In this example below, another process with PID **919636** becomes blocked. Notice that the child resides in Queue 2, which operates with a 40,000,000 nanosecond time quantum. Since the process only ran for `6,400,000` nanoseconds, this scenario dictates that PID **919636** becomes interrupted to deal with I/O operations—-not CPU operations. Now it will be placed in the **blocked queue**.

```bash
OSS: Dispatching process with PID 919636 from queue 2 at time 1:745810287
OSS: Total time spent in dispatch was 4673 nanoseconds
OSS: Receiving that process with PID 919636 ran for 6400000 nanoseconds
**OSS: Did not use its entire time quantum**
OSS: Putting process with PID 919636 into blocked queue.
```
 #### iv.) Process Unblocking:
For the child process with PID **919636**, the PCB table below states that the **event wait time** is `2` seconds, `165210287` nanoseconds.

```bash
Process Table:
Entry     Occupied     PID        StartS     StartN        ServiceS     ServiceN      EventWaitS    EventWaitN        Blocked
...
3         1            919636     1          595781211     0            36400000      2             165210287         1
...
```

Now, the system clock time is at `2` seconds, `181,441,525` nanoseconds as shown below. This system time *surpasses* PID **919636**'s wait time of `2` seconds, `165,210,287` nanoseconds. For this reason, this process becomes **unblocked** and placed back into Queue 0 (the **high-priority** queue).

```bash
OSS: Dispatching process with PID 919636 from queue 0 at time 2:181441525
OSS: Total time spent in dispatch was 9329 nanoseconds
OSS: Receiving that process with PID 919636 ran for 10000000 nanoseconds
```

#### v.) Process Termination:
As shown below, a process with PID **919635** currently resides in Queue 2 (the low-priority queue). For a complete time quantum runtime, this process must run for 40,000,000 nanoseconds. The output below states that PID **919635** ran for `-7,600,000` nanoseconds. Not only is the value below the expected time quantum, it is also a *negative number*. Every time the worker sends a *negative* runtime value back to OSS, OSS must **terminate** the process with a call to **waitpid()**.

```bash
OSS: Dispatching process with PID 919635 from queue 2 at time 1:832230261
OSS: Total time spent in dispatch was 5746 nanoseconds
OSS: Receiving that process with PID 919635 ran for -7600000 nanoseconds
**OSS: Did not use its entire time quantum**
---OSS: User #2 PID 919635 is planning to terminate.---

Before termination:
     Queue 0: (empty)
     Queue 1: (empty)
     Queue 2: 919635 919634 919633
     Queue 3: 919636

After termination:
     Queue 0: (empty)
     Queue 1: (empty)
     Queue 2: 919634 919633
     Queue 3: 919636
```
The queue illustration above shows PID **919635** disappearing from Queue 2, thus confirming the process's termination.

#### vi.) Program Termination:
PID **919634** serves as the only and final process remaining in the system. Once it terminates, there will be no more PIDs displayed in *any* queue, as shown below:

```bash
**OSS: Did not use its entire time quantum**
---OSS: User #1 PID 919634 is planning to terminate.---

Before termination:
     Queue 0: (empty)
     Queue 1: (empty)
     Queue 2: 919634
     Queue 3: (empty)

After termination:
     Queue 0: (empty)
     Queue 1: (empty)
     Queue 2: (empty)
     Queue 3: (empty)
Program successfully terminated.
```

### Example 2: Process Control Block (PCB) Table Output
Every half second of simulated system time, a Process Table prints to the console and log file as shown below:

```
OSS PID: 919632  SysClockS: 2  SysClockNano: 80011684
Process Table:
Entry     Occupied     PID        StartS     StartN        ServiceS     ServiceN      EventWaitS    EventWaitN        Blocked
0         1            919633     0          304289380     1            70000000      0             0                 0
1         1            919634     0          746930885     0            630000000     0             0                 0
2         0            0          0          0             0            0             0             0                 0
3         1            919636     1          595781211     0            36400000      2             165210287         1
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
- Applied message queues to facilitate scheduling, runtimes, blocking, and termination of child processes.
- Implemented a Multilevel Feedback Queue (MLFQ) to control the order and timeframes of child process execution.
- Implemented a probability function to determine the fate of child processes, including termination and blocking.
- Enforced times on the quantity and concurrency of processes running in the system.
- Provided clear output and visualization on all events that occur during system runtime.
  
## Tested On:
- Ubuntu 20.04.6 (LTS)
- GCC 10.5.0
- Make 4.2.1

## License:
This project is licensed under the [MIT License](LICENSE).
