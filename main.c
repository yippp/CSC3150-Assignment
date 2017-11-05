/* This is the solution to Q2.
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define FILENAME "/dev/null"

// System V semaphore relative variable
int sem_set(int sem_id);
int sem_p(int sem_id);
int sem_v(int sem_id);
int sem_del(int sem_id);


int main(int argc, char *argv[])
{
    int t1, t2, t3, t4, t5, t6;
    pid_t child1, child2, grandchild;

    t1 = atoi(argv[2]); // get time data from arguments
    t2 = atoi(argv[3]);
    t3 = atoi(argv[4]);
    t4 = atoi(argv[5]);
    t5 = atoi(argv[6]);
    t6 = atoi(argv[7]);

    // shared memory
    key_t shm_key = ftok(FILENAME, 0); // get the IPC key
    if(shm_key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shm_size = getpagesize(); // set shared memory size
    int shm_id = shmget(shm_key, shm_size, 0644 | IPC_CREAT);
    if(shm_id == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int *idata = (int *)shmat(shm_id, NULL, 0); // put idata into shared memory

    if(idata == (int *)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    memset(idata, 0, shm_size);

    key_t sem_key = shm_key; // set semaphore key
    int sem_id = semget(sem_key, 1, 0644 | IPC_CREAT); // set semaphore ID
    if(sem_id == -1){
        perror("semget");
        exit(EXIT_FAILURE);
    }

    if(sem_set(sem_id) == -1){
        perror("sem_set");
        exit(EXIT_FAILURE);
    }


    *idata = atoi(argv[1]); // get initial idata from argument
    printf("PID\trole\t\tidata\ttime\n");

    // parent process
    time_t rawtime; // time relatived variable
    struct tm * timeinfo;
    time(&rawtime); // get the current time
    timeinfo = localtime(&rawtime);

    printf("%4d\tparent\t\t\t%s\n", getpid(), asctime(timeinfo)); // print process created infomation
    fflush(stdout);

    sleep(t1);

    child1 = fork(); // creat first child process
    if (child1 < 0) {
        perror("fork first child");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0)
    {
        //first child process
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%4d\tfirst child\t\t%s\n", getpid(), asctime(timeinfo));
        fflush(stdout);

        sleep(t3);

        if(sem_p(sem_id) == -1){ // semaphore wait
            perror("sem_p(first child)");
            exit(EXIT_FAILURE);
        }

        *idata += 1;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%4d\tfirst child\t%d\t%s\n", getpid(), *idata, asctime(timeinfo)); // print procee terminated information

        if(sem_v(sem_id) == -1){ // semaphore signal
            perror("sem_v(first child)");
            exit(EXIT_FAILURE);
        }

        if(shmdt(idata) == -1){ // disconnect shared memory
            perror("shmdtv(first child)");
            exit(EXIT_FAILURE);
        }

        fflush(stdout);
        exit(0); // terminate first child
    }


    else
    {
        sleep(t2);
        child2 = fork(); //create second child
        if (child1 < 0){
            perror("fork second child");
            exit(EXIT_FAILURE);
        }
        if (child2 == 0)
        {
            // second child process
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            printf("%4d\tsecond child\t\t%s\n", getpid(), asctime(timeinfo)); // print process created infomation
            fflush(stdout);

            sleep(t5);

            grandchild = fork(); // grandchild proces
            if (child1 < 0){
                perror("fork grandchild");
                exit(EXIT_FAILURE);
            }
            if (grandchild == 0)
            {
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                printf("%4d\tgrandchild\t\t%s\n", getpid(), asctime(timeinfo)); // print procee terminated information
                fflush(stdout);

                sleep(t6);

                if(sem_p(sem_id) == -1){ // semaphore wait
                    perror("sem_p(grandchild)");
                    exit(EXIT_FAILURE);
                }

                *idata += 1;

                time(&rawtime);
                timeinfo = localtime(&rawtime);
                printf("%4d\tgrandchild\t%d\t%s\n", getpid(), *idata, asctime(timeinfo));

                if(sem_v(sem_id) == -1){ // semaphore signal
                    perror("sem_v(grandchild)");
                    exit(EXIT_FAILURE);
                }

                if(shmdt(idata) == -1){ // disconnect shared memory
                    perror("shmdt(grandchild)");
                    exit(EXIT_FAILURE);
                }

                fflush(stdout);
                exit(0); // terminate grandchild
            }


            else{
                // second child process

                sleep(t4);

                if(sem_p(sem_id) == -1){ // semaphore wait
                    perror("sem_p(second child)");
                    exit(EXIT_FAILURE);
                }

                *idata += 1;


                time(&rawtime);
                timeinfo = localtime(&rawtime);
                printf("%4d\tsecond child\t%d\t%s\n", getpid(), *idata, asctime(timeinfo));// print procee terminated information

                if(sem_v(sem_id) == -1){ // semaphore signal
                    perror("sem_v(second child)");
                    exit(EXIT_FAILURE);
                }

                if(shmdt(idata) == -1){ // disconnect shared memory
                    perror("shmdt(second child)");
                    exit(EXIT_FAILURE);
                }

                fflush(stdout);
                exit(0); // termiate second child
            }
        }


        else
        {
            // parent process
            if(sem_p(sem_id) == -1){ // semaphore wait
                perror("sem_p(parent)");
                exit(EXIT_FAILURE);
            }

            *idata += 1;


            time(&rawtime);
            timeinfo = localtime(&rawtime);
            printf("%4d\tparent\t\t%d\t%s\n", getpid(), *idata, asctime(timeinfo)); // print procee terminated information

            if(sem_v(sem_id) == -1){ // semaphore signal
                perror("sem_v(parent)");
                exit(EXIT_FAILURE);
            }

            if(shmdt(idata) == -1){ // disconnect shared memory
                perror("shmdt(parent)");
                exit(EXIT_FAILURE);
            }

            fflush(stdout);
            return 0; // terminate parent process
        }
    }

    return 0;
}


int sem_set(int sem_id){
    union semun sem_u;
    sem_u.val = 1;

    if(semctl(sem_id, 0, SETVAL, sem_u) == -1){
        return -1;
    }else{
        return 0;
    }
}

int sem_p(int sem_id){
    struct sembuf sem_b[1];
    sem_b[0].sem_num = 0;
    sem_b[0].sem_op = -1;
    sem_b[0].sem_flg = SEM_UNDO;

    if(semop(sem_id, sem_b, 1) == -1){
        return -1;
    }else{
        return 0;
    }
}

int sem_v(int sem_id){
    struct sembuf sem_b[1];
    sem_b[0].sem_num = 0;
    sem_b[0].sem_op = 1;
    sem_b[0].sem_flg = SEM_UNDO;

    if(semop(sem_id, sem_b, 1) == -1){
        return -1;
    }else{
        return 0;
    }
}

int sem_del(int sem_id){
    if(semctl(sem_id, 0, IPC_RMID) == -1){
        return -1;
    }else{
        return 0;
    }
}
