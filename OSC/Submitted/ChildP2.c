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

#define SIZE_OF_MEMORY sizeof(int)
#define SHARED_MEMORY_NAME "storage"

int main(int argc, char *argv[]){
    //virables initialization and semaphore open
    sem_t *child1, *child2, *parent;   
    child1 = sem_open("child1", O_CREAT, 0666, 1); 
    child2 = sem_open("child2", O_CREAT, 0666, 0); 

    //shared memory initialization/opening
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT,0666);
    
    int *stored = (int *)mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,0);

    //main for loop with calculations
    int i;
    for(i = 2; i < 21; i+=2){
        sem_wait(child2);
	    stored[i] = stored[i-1]-10;
        sem_post(child1);
    }
    
    //closing semaphores and unlinkind shared memory
    sem_close(child1);
    sem_close(child2);
    close(shm_fd);
    sem_unlink("child1");
    sem_unlink("child2");
    munmap(stored, SIZE_OF_MEMORY);
    shm_unlink(SHARED_MEMORY_NAME);

    return 0;
}
