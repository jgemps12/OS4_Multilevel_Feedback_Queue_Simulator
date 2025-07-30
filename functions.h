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
void slowDownProgram();

// For process table operations.
int addToProcessTable(pid_t);
int findIndexInProcessTable(pid_t);
void addServiceTimeToProcessTable(int);
void addWaitTimeToProcessTable(long long int, int);
void possiblyUnblockChild(MultiLevelQueue *);
void removeFromProcessTable(pid_t);
void printProcessTable();
void printProcessTableToLogfile();

// For message passing operations.
void sendMessageToUSER();
void receiveMessageFromUSER(int);

// For guiding the user.
void printHelpMessage();

// For terminating the program.
void detachAndClearSharedMemory();
void removeMessageQueue();
void periodicallyTerminateProgram(int);

