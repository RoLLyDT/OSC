/***  
Maksim KOZLOV 20219332
***/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "coursework.h"
#include "linkedlist.h"

void bubbleSort();

int main(){
    //variables initialization
    int turnaroundTime[NUMBER_OF_PROCESSES], responseTime[NUMBER_OF_PROCESSES];
    int totalTAT = 0, totalResponseTime = 0;
    struct element *ptr, *head = NULL, *tail = NULL;
    struct process *p[NUMBER_OF_PROCESSES];
    double avgRt, avgTAT;

    //for loop to generate processes
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++)
    {
        p[i] = generateProcess(); // change to variable of type struct process isntead of array
        runNonPreemptiveJob(p[i], &p[i]->oTimeCreated, &p[i]->oMostRecentTime);
        addLast(p[i], &head, &tail);
    }

    //sorting algorithm run
    bubbleSort(head); 

    //main calculations + moving to next element
    ptr = head;
    int tempBurstTime = 0;
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++)
    {
        responseTime[i] = 0;
        if (i != 0)
            responseTime[i] = responseTime[i-1] + tempBurstTime;
        tempBurstTime = ((struct process *)(ptr->pData))->iPreviousBurstTime; //p[i]->iProcessId == ((struct process *)(ptr->pData))->iProcessId
        turnaroundTime[i] = responseTime[i] + tempBurstTime;
        ptr = ptr -> pNext;
        totalResponseTime += responseTime[i];
        totalTAT += turnaroundTime[i];
    }
    //average calulations
    avgRt = (double)totalResponseTime/(double)NUMBER_OF_PROCESSES;
    avgTAT = (double)totalTAT/NUMBER_OF_PROCESSES;

    ptr = head;
    FILE *f = fopen("TASK1a.txt", "w"); //creating .txt file

    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++) //output
    {
        fprintf(f, "Process Id = %d, ", ((struct process *)(ptr -> pData)) -> iProcessId); // Purple bracks = p[i]
        fprintf(f, "Previous Burst Time = %d, ", ((struct process *)(ptr -> pData)) -> iPreviousBurstTime);
        fprintf(f, "Remaining Burst Time = %d, ", ((struct process *)(ptr -> pData)) -> iRemainingBurstTime);
        fprintf(f, "Response Time = %d, ", responseTime[i]);
        fprintf(f, "Turnaround Time = %d\n", turnaroundTime[i]);
        ptr = ptr -> pNext ;
        removeFirst(&head, &tail);
    }
    fprintf(f, "Average Response Time: %f \n", avgRt);
    fprintf(f, "Average Turnaround Time: %f \n", avgTAT);

    fclose(f);
}

//bubble sort realisation for our case
void bubbleSort(struct element *head){
    struct element *ptr = NULL, *ptr2 = NULL;
    struct process *temp = NULL;

    ptr2 = head;
    while (ptr2 != NULL)
    {
        ptr = head;
        while (ptr -> pNext != NULL)
        {
            if (((struct process *)(ptr -> pData))->iPreviousBurstTime > 
                ((struct process *)(ptr -> pNext -> pData))->iPreviousBurstTime)
                {
                    temp = ptr -> pData;
                    ptr -> pData = ptr -> pNext -> pData;
                    ptr -> pNext -> pData = temp;  
                }
            ptr = ptr -> pNext;
        }
        ptr2 = ptr2 -> pNext;
    }
}