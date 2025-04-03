/* Jesse Gempel
 * 3/18/2025
 * Professor Mark Hauschild
 * CMP SCI 4760-001
*/


// The user.c file works with CHILD processes.
// It prints out child and parent process IDs, as well as child process and termination times, to the user for each iteration.


#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>


// Starting a memory segment for system clock seconds.
#define SHMKEY1 42069
#define INT_BUFFER_SIZE sizeof(int)


// Starting a memory segment for system clock nanoseconds.
#define SHMKEY2 42070
#define LONG_BUFFER_SIZE sizeof(long int)


// Starting a memory segment for a log file to validate ftok() function.
#define SHMKEY3 42071
#define LOGFILE_BUFFER_SIZE 105


// Permissions for memory queue.
#define PERMISSIONS 0644


// Holds message queue information.
typedef struct messageBuffer {
   long int messageType;
   char stringData[100];
   long int quantumData;
} messageBuffer;


// Initializes information for message buffer.
messageBuffer receiveBuffer;
messageBuffer sendBuffer;
int messageQueueID;
key_t key;


// Gathers logfile name from oss.c to validate ftok() function in user.c (this file).
int childProcessTimeSeconds;
int childTerminationTimeSeconds;
long int childProcessTimeNano;
long int childTerminationTimeNano;


// Logfile pointer.
char *logfileFP = NULL;


// Function prototypes.
void initializeMessageQueue();
void sendMessageToOSS();
void receiveMessageFromOSS();

int getTerminationTimeSeconds (int processSeconds, int systemClockSeconds) {
   return processSeconds + systemClockSeconds;
}


// The point in simulated system time when child terminates.
long int getTerminationTimeNano (long int processNanoseconds, long int systemClockNano, int *termSeconds) {
   long int terminationNano = processNanoseconds + systemClockNano;
   long int oneBillionSeconds = 1000000000;
   
   while (terminationNano >= oneBillionSeconds) {
      (*termSeconds)++;
      terminationNano -= oneBillionSeconds;
   }

   return terminationNano;
}

// Amount of time that child process stays in system.
int getElapsedTimeSeconds (int startingTimeSeconds, int currentTimeSeconds) {
   return currentTimeSeconds - startingTimeSeconds;
}


int main(int argc, char** argv) {
  
   // Creates two shared memory identifiers (and one for a log file).
   int secondsShmid = shmget(SHMKEY1, INT_BUFFER_SIZE, 0777);
   long int nanoShmid = shmget(SHMKEY2, LONG_BUFFER_SIZE, 0777);
   int logfileShmid = shmget(SHMKEY3, LOGFILE_BUFFER_SIZE, 0777);


   // Attaches the system time and log file into shared memory.
   int *sharedSeconds = (int *)shmat(secondsShmid, 0, 0);
   long int *sharedNanoseconds = (long int *)shmat(nanoShmid, 0, 0);
   logfileFP = (char *)shmat(logfileShmid, 0, 0);


   // Used for constant system time updated from shared memory.
   int systemClockSeconds = *sharedSeconds;
   long int systemClockNano = *sharedNanoseconds;


   // Used for termination time calculations. Will not be updated.
   int initialSystemClockSecs = *sharedSeconds;
   long int initialSystemClockNano = *sharedNanoseconds;        
   
   
   // processSelection uses numbers 1-3 to determine child process's outcome.
   // probabilityValue randomly chooses between 1 and 100 to determine processSelection value.
   srand(time(NULL));
   int probabilityValue = 0;
   int processSelection = -1;
   int timeQuantumFraction = 0;


   initializeMessageQueue();


   // If executing ./user [timeLimitForChildren] [timeLimitNanoseconds], user must enter two integers after ./user.
   // Otherwise, present error and terminate program.
   if (argc != 3) {
      printf("ERROR in 'user.c': You must enter ./user followed by two integers.\n\n");

      printf("Integer 1: the maximum time limit for a child process (in seconds).\n");
      printf("Integer 2: the # of nanoseconds AFTER seconds time limit has been reached.\n\n");

      printf("Example: ./user 5 200000\n\n");

      exit(-1);
   }


   // Store user input that specifies the amount of time that the loop below will iterate.
   if (argc == 3) { 
      childProcessTimeSeconds = atoi(argv[1]);
      childProcessTimeNano = atoi(argv[2]);
   }


   // Determines the system time that the child process should terminate.
   childTerminationTimeSeconds = getTerminationTimeSeconds(childProcessTimeSeconds, initialSystemClockSecs);
   childTerminationTimeNano = getTerminationTimeNano(childProcessTimeNano, initialSystemClockNano, &childTerminationTimeSeconds);


  
   // Print starting message.
   printf("USER PID: %d   PPID: %d  SysClockS: %d  SysClockNano: %ld  TermTimeS: %d  TermTimeNano: %ld ---Just starting\n", getpid(), getppid(), systemClockSeconds, systemClockNano, childTerminationTimeSeconds, childTerminationTimeNano);


   // Keep track of how many times the do-while loop iterates.
   long int iterations = 0;


   do {
      // Receives a message from the parent.
      receiveMessageFromOSS();

      printf("\nreceiveBuffer.quantumData (from user.c): %ld\n", receiveBuffer.quantumData);
      

      // Compare # of seconds before and after shared memory is re-read.
      int secondsBeforeMemRead = systemClockSeconds;                                              // Before read.
      systemClockSeconds = *sharedSeconds;                                                        // During read.
      int secondsAfterMemRead = systemClockSeconds;                                               // After read.

      systemClockNano = *sharedNanoseconds;

      int secondsRan = getElapsedTimeSeconds(initialSystemClockSecs, systemClockSeconds);
 

      // Slow down program to prevent race conditions between Process Table and printf() message times (for oss.c and user.c, respectively).
      int i;
      for (i = 0; i < 100000000; i++) {
         //  Do nothing.
      }

      probabilityValue = rand() % (100 + 1);

      if (probabilityValue >= 1 && probabilityValue <= 94) {
         processSelection = 1;
      }
      else if (probabilityValue >= 95 && probabilityValue <= 99) {
         processSelection = 2;
      }
      else if (probabilityValue == 100) {
         processSelection = 3;
      }

      printf("probabilityValue: %d\n", probabilityValue);
      printf("processSelection: %d\n", processSelection);

      // If a child is about to terminate, print an exit message.
     // if (systemClockSeconds >= childTerminationTimeSeconds && 
         //   (systemClockSeconds > childTerminationTimeSeconds || systemClockNano >= childTerminationTimeNano)) {
      switch (processSelection) {
         case 1:
            sendBuffer.messageType = getpid();
            sendBuffer.quantumData = 10000000;

            sendMessageToOSS();

            printf("\nsendBuffer.quantumData (from user.c): %ld\n", sendBuffer.quantumData);
            iterations++;
         
	    printf("USER PID: %d   PPID: %d  SysClockS: %d  SysClockNano: %ld  TermTimeS: %d  TermTimeNano: %ld ---%ld iteration(s) have passed since starting\n", getpid(), getppid(), systemClockSeconds, systemClockNano, childTerminationTimeSeconds, childTerminationTimeNano, iterations);

	    break;
	 
	 case 2:
	    timeQuantumFraction = rand() % (99 + 1);

            sendBuffer.messageType = getpid();
            sendBuffer.quantumData = (timeQuantumFraction * 10000000) / 100;

            sendMessageToOSS();

            printf("\nsendBuffer.quantumData (from user.c): %ld\n", sendBuffer.quantumData);
            iterations++;

            printf("USER PID: %d   PPID: %d  SysClockS: %d  SysClockNano: %ld  TermTimeS: %d  TermTimeNano: %ld ---%ld iteration(s) have passed since starting\n", getpid(), getppid(), systemClockSeconds, systemClockNano, childTerminationTimeSeconds, childTerminationTimeNano, iterations);

            break;


	 case 3:
            timeQuantumFraction = rand() % (99 + 1);

	    sendBuffer.messageType = getpid();
            sendBuffer.quantumData = (-1 * timeQuantumFraction * 10000000) / 100;

            snprintf(sendBuffer.stringData, sizeof(sendBuffer.stringData), "Message received to child. Now sending to parent. Child is now terminating.");
            sendMessageToOSS();

            printf("\n\nsendBuffer.quantumData (from user.c): %ld\n\n", sendBuffer.quantumData);

            printf("USER PID: %d   PPID: %d  SysClockS: %d  SysClockNano: %ld  TermTimeS: %d  TermTimeNano: %ld ---Terminating\n", getpid(), getppid(), systemClockSeconds, systemClockNano, childTerminationTimeSeconds, childTerminationTimeNano);

	    
	    break;
      }
      if (processSelection == 3) {
         break;
      }
/*
      if (iterations >= 10) {
	 sendBuffer.messageType = getpid();
         sendBuffer.quantumData = -10000000;

         snprintf(sendBuffer.stringData, sizeof(sendBuffer.stringData), "Message received to child. Now sending to parent. Child is now terminating.");
	 sendMessageToOSS();

	 printf("\n\nsendBuffer.quantumData (from user.c): %ld\n\n", sendBuffer.quantumData);

         printf("USER PID: %d   PPID: %d  SysClockS: %d  SysClockNano: %ld  TermTimeS: %d  TermTimeNano: %ld ---Terminating\n", getpid(), getppid(), systemClockSeconds, systemClockNano, childTerminationTimeSeconds, childTerminationTimeNano);
 
	 break;
      }


      // If a child is still running, print a progress message with # of iterations.
      else {
         sendBuffer.messageType = getpid();
         sendBuffer.quantumData = 10000000;

	 sendMessageToOSS();

  	 printf("\n\nsendBuffer.quantumData (from user.c): %ld\n\n", sendBuffer.quantumData); 
	 iterations++;
         printf("USER PID: %d   PPID: %d  SysClockS: %d  SysClockNano: %ld  TermTimeS: %d  TermTimeNano: %ld ---%ld iteration(s) have passed since starting\n", getpid(), getppid(), systemClockSeconds, systemClockNano, childTerminationTimeSeconds, childTerminationTimeNano, iterations);
         
      }*/
   }
   while (1);
  

   // Detach shared memory.
   shmdt(sharedSeconds);
   shmdt(sharedNanoseconds);
   shmdt(logfileFP);


   return EXIT_SUCCESS;

}


// Attempts to set up a message queue.
void initializeMessageQueue() {
   if ((key = ftok(logfileFP, 1)) == -1) {
      printf("ERROR in user.c: problem with ftok() function.\n");
      printf("Cannot access a key for message queue initialization.\n\n");

      exit(-1);
   }

   if ((messageQueueID = msgget(key, PERMISSIONS)) == -1) {
      printf("ERROR in user.c: problem with msgget() function.\n");
      printf("Cannot acquire a message queue ID for initialization.\n\n");

      exit(-1);
   }
}


// msgsnd() operations.
void sendMessageToOSS() {
   if (msgsnd(messageQueueID, &sendBuffer, sizeof(messageBuffer) - sizeof(long int), 0) == -1) {
      printf("ERROR in user.c: Problem with msgsnd() function.\n");
      printf("Cannot send message to oss.c.\n\n");

      exit(-1);
   }
}


// msgrcv() operations.
void receiveMessageFromOSS() {
   if (msgrcv(messageQueueID, &receiveBuffer, sizeof(messageBuffer), getpid(), 0) == -1) {
      printf("ERROR in user.c: Problem with msgrcv() function.\n");
      printf("Cannot receive message from oss.c.\n\n");

      exit(-1);
   }
}
