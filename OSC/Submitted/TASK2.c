/***  
Maksim KOZLOV 20219332
***/

#include <fcntl.h> /* For O_* constants */
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <wait.h>

#define SIZE_OF_MEMORY sizeof(int)
#define SHARED_MEMORY_NAME "storage"

int main(int argc, char *argv[]){
    //virables initialization and semaphore open
    pid_t pid = 0, wpid;
    int i, n, status = 0, ID = 1;
    sem_t *child1, *child2, *parent;   
    child1 = sem_open("child1", O_CREAT, 0666, 1); 
    child2 = sem_open("child2", O_CREAT, 0666, 0); 

    //shared memory initialization/opening
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT,0666);
    ftruncate(shm_fd, SIZE_OF_MEMORY);
    
    int *stored = (int *)mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,0);

    srand(time(NULL)); //to generate all the time new number
    stored[0] = rand() %20 + 1;
    printf("The RandInt = %d, created by the parent process\n", stored[0]);

    //creating of 2 child processes using fork
    for (i = 0; i < 2; i++){
        pid = fork();
        if (pid < 0){
            printf("couldn't create process\n");
            exit(1);
        }
        else if (pid == 0){
            if (i == 0){
	            execvp("./ChildP1",NULL);
	            //exit(0);
            }
            if (i == 1){
	            execvp("./ChildP2",NULL);
	            //exit(0);
            }  
	    return 0;
        }
    }
    while((wpid = wait(&status)>0)); //wait until every child process

    for (i = 1; i < 11; i++) //output
    {
        printf("In round %d, RandInt = %d, child process 1, \n", i, stored[ID++]);
        printf("In round %d, RandInt = %d, child process 2, \n\n", i, stored[ID++]);
    }

    //closing semaphores and unlinkind shared memory
    close(shm_fd);
    sem_close(child1);
    sem_close(child2);
    sem_unlink("child1");
    sem_unlink("child2");
    shm_unlink(SHARED_MEMORY_NAME);
    munmap(stored, SIZE_OF_MEMORY);


 return 0;
}
