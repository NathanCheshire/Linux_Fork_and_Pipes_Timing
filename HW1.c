#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sched.h>
#include <assert.h>


//given timeval struct t, we convert it to nano seconds
long ns(struct timeval t) { 
  return((t.tv_sec * 1000000 + t.tv_usec) * 1000);
}

int main() {
    //var init
    int x;
    int y;
    
    struct timeval t1;
    struct timeval t2;

    int loopyLoopNum = 1000000;

    //-------------------system call time------------------------------------------------------------

    //start time
    gettimeofday(&t1,NULL);

    for (x = 0 ; x < loopyLoopNum ; x++) {
        y = getpid();
    }

    //end time
    gettimeofday(&t2,NULL);

    //calculate average of all the calls
    float avgTimeSysCall = (ns(t2) - ns(t1))/(loopyLoopNum * 1.0);

    avgTimeSysCall /= 100;

    printf("avg time for getpid: %f u_sec\n",avgTimeSysCall);

    //-------------------context switch--------------------------------------------------------------
 
    //set to hold cpus
    cpu_set_t cSet;

    //clear set
    CPU_ZERO(&cSet);

    //add set to it on core 0 so that both of the threads run on the same core which is the point since we are timing context swtiching
    CPU_SET(0, &cSet);

    //int arrays of size 2 that holds ints which correspond to the two sides of the pipe
    int pipeOne[2]; //0 is read end, 1 is write end
    int pipeTwo[2]; //0 is read end, 1 is write end

    //establish pipe with both sides for both arrays and test for error
    if (pipe(pipeOne) == -1 || pipe(pipeTwo) == -1) {
        printf("pipe error first");
        exit(EXIT_FAILURE);
    }

    //processes usually create a pipe just before it forks one or more child processes.
    //the pipe is then used for communication between the processes

    //process duplication and cpid holds the child ID
    //fork calls can be assigned to the pid_t data type
    pid_t cpid = fork();

    //after fork call, both processes are running at the same time
    //one is performing zb read and writes to and from one end of the pipe, and the other is doing the same
    //by doing this n number of times, we can divide the total time it took by n
    //at the end to get our average cost of a context swtich between n context swtiches.

    //fork failed
    if (cpid == -1) {
        printf("fork error");
        exit(EXIT_FAILURE);
    } 

    //child pid since fork means both processes have the same code so make sure only the child runs this section
    else if (cpid == 0) {

        //makes sure thread runs on cpu 0 by setting the cpu affinity mask, of the thread whos ID is pid, to the value specified by &set
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &cSet) == -1)
            exit(EXIT_FAILURE);


        //zero byte read and zero byte write using child process 'n' times
        for (size_t i = 0; i < loopyLoopNum; i++) {
            //zero byte read from read end of first pipe
            read(pipeOne[0], NULL, 0);
            //zero byte write to write end of second pipe
            write(pipeTwo[1], NULL, 0);
        }
    } 

    //parent pid since fork means both processes have the same code so make sure only the parent runs this section
    else {
        //makes sure thread runs on cpu 0 by setting the cpu affinity mask, of the thread whos ID is pid, to the value specified by &set
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &cSet) == -1)
            exit(EXIT_FAILURE);

        //time val t1 holds start time
        gettimeofday(&t1, NULL);

        //----------------------this here is the true context swtiching measurement since the CPU switches 
        //----------------------between this loop and the above loop for the child process

        //zero byte read and write using the parent process 'n' times
        for (size_t i = 0; i < loopyLoopNum; i++) {
            //zero byte write to write end of first pipe
            write(pipeOne[1], NULL, 0);
            //zero byte read from read end of second pipe
            read(pipeTwo[0], NULL, 0);
        }

        //time val t2 holds end time
        gettimeofday(&t2, NULL);

        //calculate time for context switch in usec                  f cast    end seconds            end u sec    start seconds          start u sec  avg of con sw's
        printf("Avg time for cntx switch: %f u sec\n", (float) ((t2.tv_sec * loopyLoopNum + t1.tv_usec) - (t1.tv_sec * loopyLoopNum - t1.tv_usec)) / loopyLoopNum);
    }

    return 0;
}