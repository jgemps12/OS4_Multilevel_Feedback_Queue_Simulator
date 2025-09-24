#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>

// For process table.
#define PROCESS_COUNT 20

// For memory queue.
#define PERMISSIONS 0644

// Starting a multilevel feedback queue.
#define MAX_SIZE 1000
#define QUEUE_COUNT 4

/*********************************STRUCTS************************************/
// For process table operations.
struct PCB {
   int occupied;                            // Is the entry in the process table empty (0) or full (1)?
   pid_t processID;                         // Child's process ID.
   int startSeconds;                        // Time when a process FORKED (in seconds).
   long int startNanoseconds;               // Time when a process FORKED (in nanoseconds).
   int serviceTimeSeconds;                  // Total time when a process was SCHEDULED (in seconds).
   long int serviceTimeNanoseconds;         // Total time when a process was SCHEDULED (in nanoseconds).
   int eventWaitSeconds;                    // Time when a process becomes UNBLOCKED (in seconds).
   long long int eventWaitNanoseconds;      // Time when a process becomes UNBLOCKED (in nanoseconds).
   int blocked;                             // Is the process blocked (1) or unblocked (0)?
};
extern struct PCB processTable[PROCESS_COUNT];

// Sets up for different queues for multilevel feedback scheduling.
typedef struct MultiLevelQueue {
   int processEntries[MAX_SIZE];
   int front;
   int rear;
} MultiLevelQueue;

// Enumerates queue levels. 
enum Level {
   HIGH_PRIORITY,
   MED_PRIORITY,
   LOW_PRIORITY,
   BLOCKED
};

// Holds message queue information.
typedef struct messageBuffer {
   long int messageType;
   char stringData[100];
   long int quantumData;
} messageBuffer;

/****************************GLOBAL VARIABLES********************************/
// For log file operations.
extern char logfile[105];
extern char suffix[];
extern FILE *logOutputFP;
extern char *logfileFP;

// For shared memory operations.
extern int secondsShmid;
extern long int nanoShmid;
extern int logfileShmid;

// For message buffer operations.
extern messageBuffer sendBuffer;
extern messageBuffer receiveBuffer;
extern int messageQueueID;
extern key_t key;

// For system clock operations.
extern int systemClockSeconds;
extern long long int systemClockNano;
extern long long int systemNanoOnly;
extern long int systemClockIncrement;

// For system clock sharing between processes.
extern int *secondsShared;
extern long int *nanosecondsShared;

// For time conversions.
extern long int oneMillionNanoseconds;
extern long int halfBillionNanoseconds;
extern long int oneBillionNanoseconds;
extern long int oneQuarterSecond;
extern long int hundredMS;

// For process table operations. 
extern int lastTablePrintSeconds;
extern long int lastTablePrintNano;

// For feedback queue operations.
extern int currentChildIndex;

/*************************FUNCTION PROTOTYPES********************************/
//For initialization.
void initializeLogfile();
void initializeMessageQueue();
void initializeFeedbackQueue(MultiLevelQueue *);

// For feedback queue.
bool isQueueEmpty(MultiLevelQueue *);
void enqueue(MultiLevelQueue *, pid_t);
pid_t dequeue(MultiLevelQueue *);
pid_t peekQueue(MultiLevelQueue *);
void printAllFeedbackQueues(MultiLevelQueue *);
void printOneQueue(MultiLevelQueue *);

// For system clock/time operations.
long long int incrementClock(int *, long long int *, int);
long long int convertSystemTimeToNanosecondsOnly (int *, long long int *);
long long int determineNextLaunchNanoseconds(long long int, long long int);
int determineDispatchTime();
long long int determineEventWaitTime(int, int, long long int);
int determineTimeQuantum(int);
void incrementIfAllChildrenAreBlocked();
void slowDownProgram();

// For process table operations.
int addToProcessTable(pid_t);
int findIndexInProcessTable(pid_t);
void addServiceTimeToProcessTable(int);
void addWaitTimeToProcessTable(long long int, int);
int possiblyUnblockChild(MultiLevelQueue *);
void removeFromProcessTable(pid_t);
void printProcessTable();
void printProcessTableToLogfile();

// For message passing operations.
void sendMessageToUSER();
void receiveMessageFromUSER(int);

// For guiding the user.
void printHelpMessage();

// For program statistics operations.
long long int calculateReadyStateTime(long long int, int);
double getTotalReadyStateTime(long long int, int);
long long int calculateBlockedStateTime(long long int, int);
double getTotalBlockedStateTime(long long int, int);
long long int calculateIdleTime(long long int, long long int);
double getTotalIdleTime(long long int);
double getUtilizationOfCPU(long long int);

// For terminating the program.
void detachAndClearSharedMemory();
void removeMessageQueue();
void printProgramSummary(int, long long int, long long int, long long int);
void periodicallyTerminateProgram(int);


#endif
