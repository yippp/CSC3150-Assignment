#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define FILENAME "/dev/null"


int main(int argc, char *argv[])
{
    //int *idata=10;
    int t1, t2, t3, t4, t5, t6;
    pid_t child1, child2, grandchild;
//    t1 = 0;
//    t2 = 0;
//    t3 = 0;
//    t4 = 0;
//    t5 = 0;
//    t6 = 0;
//    idata = int(argv[2])；
    t1 = atoi(argv[2]);
    printf("%d",t1);
    t2 = atoi(argv[3]);
    t3 = atoi(argv[4]);
    t4 = atoi(argv[5]);
    t5 = atoi(argv[6]);
    t6 = atoi(argv[7]);


    key_t shm_key = ftok(FILENAME, 0);
    int shm_size = getpagesize();
    int shm_id = shmget(shm_key, shm_size, 0644 | IPC_CREAT);

    if(shm_id == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *idata = (int *)shmat(shm_id, NULL, 0);

    if(idata == (void *)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }


    *idata = atoi(argv[1]);

    printf("PID\trole\t\tidata\ttime\n");

    // parent process
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime); // get the current time
    timeinfo = localtime(&rawtime);

    printf("%4d\tparent\t\t\t%s\n", getpid(), asctime(timeinfo));
    fflush(stdout);

    sleep(t1);


    child1 = fork(); // first child process
    if(child1 < 0){
        perror("fork first child");
        exit(EXIT_FAILURE);
    }
    if (child1 == 0)
    {

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%4d\tfirst child\t\t%s\n", getpid(), asctime(timeinfo));
        fflush(stdout);

        sleep(t3);

        *idata += 1;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%4d\tfirst child\t%d\t%s\n", getpid(), *idata, asctime(timeinfo));
        fflush(stdout);
        exit(0);
    }


    else
    {
        sleep(t2); // second child process
        child2 = fork();
        if(child1 < 0){
            perror("fork second child");
            exit(EXIT_FAILURE);
        }
        if (child2 == 0)
        {

            sleep(t5);
            grandchild = fork(); // grandchild proces
            if(child1 < 0){
                perror("fork grandchild");
                exit(EXIT_FAILURE);
            }
            if (grandchild == 0)
            {
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                printf("%4d\tgrandchild\t\t%s\n", getpid(), asctime(timeinfo));
                fflush(stdout);

                sleep(t6);

                *idata += 1;

                time(&rawtime);
                timeinfo = localtime(&rawtime);
                printf("%4d\tgrandchild\t%d\t%s\n", getpid(), *idata, asctime(timeinfo));
                fflush(stdout);
                exit(0);
            }


            else{
                // second child process
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                printf("%4d\tsecond child\t\t%s\n", getpid(), asctime(timeinfo));
                fflush(stdout);

                sleep(t4);

                *idata += 1;

                time(&rawtime);
                timeinfo = localtime(&rawtime);
                printf("%4d\tsecond child\t%d\t%s\n", getpid(), *idata, asctime(timeinfo));
                fflush(stdout);
                exit(0);
            }
        }


        else
        {
            // parent process
            *idata += 1;

            time(&rawtime);
            timeinfo = localtime(&rawtime);
            printf("%4d\tparent\t\t%d\t%s\n", getpid(), *idata, asctime(timeinfo));
            fflush(stdout);
            return 0;
        }
    }

    return 0;
}
