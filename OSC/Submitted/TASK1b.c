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
int getBurstTime();

int main(){
    //variables initialization
    struct process *p[NUMBER_OF_PROCESSES];
    struct element *ptr;
    struct element *head = NULL, *tail = NULL;
    struct element *headN = NULL, *tailN = NULL;
    int turnaroundTime = 0, responseTime = 0;
    int totalTAT = 0, totalResponseTime = 0;
    double avgRt, avgTAT;
    FILE *f = fopen("TASK1b.txt", "w");

    //for loop to generate processes
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++)
    {
        p[i] = generateProcess();
        addLast(p[i], &head, &tail);
    }

    //sorting algorithm run
    bubbleSort(head);
    ptr = head;
    int check = ((struct process *)(ptr -> pData)) -> iPriority;
    fprintf(f, "PROCESS LIST: \n");
    //for loop of printing after sort alg
    for (int i = 0; i < NUMBER_OF_PROCESSES; i++)
    {
        if (((struct process *)(ptr -> pData)) -> iPriority != check || i == 0)
            fprintf(f, "Priority %d \n", ((struct process *)(ptr -> pData)) -> iPriority);
        fprintf(f, "\t Process Id = %d, ", ((struct process *)(ptr -> pData)) -> iProcessId);
        fprintf(f, "Priority = %d, ", ((struct process *)(ptr -> pData)) -> iPriority);
        fprintf(f, "Initial Burst Time = %d, ", ((struct process *)(ptr -> pData)) -> iInitialBurstTime);
        fprintf(f, "Remaining Burst Time = %d, \n", ((struct process *)(ptr -> pData)) -> iRemainingBurstTime);
        check = ((struct process *)(ptr -> pData)) -> iPriority;
        ptr = ptr -> pNext;
    }
    fprintf(f, "END \n\n");
    ptr = head;

    //while loop section with adding/removing elements in the lists and main calculations + printing
    long int time = 0;
    while (head != NULL)
    {
        headN = NULL; 
        tailN = NULL;
        addLast(removeFirst(&head, &tail), &headN, &tailN);
        int priority = ((struct process *)(headN->pData)) -> iPriority;
        int cnt = 1, cntDone = 0;
        while(head != NULL && (((struct process *)(head->pData)) -> iPriority == priority))//adding in the new list+removing from the old elements 
        {
            addLast(removeFirst(&head, &tail), &headN, &tailN);
            cnt++;
        }

        ptr = headN; 
        int used[NUMBER_OF_PROCESSES] = {0};
        int i = 0;
        while(cntDone < cnt) //while loop section that checks if calculation was done
        {
            if(((struct process *)(ptr -> pData)) -> iRemainingBurstTime > 0 && used[i] != -1) 
            {
                int burstTime = getBurstTime(((struct process *)(ptr -> pData)));
                time += burstTime; 
                runPreemptiveJob(((struct process *)(ptr -> pData)), 
                                &((struct process *)(ptr -> pData)) -> oTimeCreated,
                                &((struct process *)(ptr -> pData)) -> oMostRecentTime); //preemptive job running
                fprintf(f, "Process Id = %d, Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d",
                    ((struct process *)(ptr -> pData)) -> iProcessId,
                    ((struct process *)(ptr -> pData)) -> iPriority,
                    ((struct process *)(ptr -> pData)) -> iPreviousBurstTime,
                    ((struct process *)(ptr -> pData)) -> iRemainingBurstTime);
                if (used[i] == 0)
                {
                    totalResponseTime += time-burstTime;
                    fprintf(f, ", Response time = %d", time-burstTime);
                }
                if(((struct process *)(ptr -> pData)) -> iPreviousBurstTime <= burstTime) 
                {
                    totalTAT += time;
                    fprintf(f, ", Turnaround Time = %d", time);
                }
                fprintf(f, "\n");
                used [i] = 1;
            }
            else if (used[i] != -1) 
            {
                used[i] = -1;
                cntDone++;
            }
            if (ptr == tailN)
                ptr = headN;
            else 
                ptr = ptr->pNext;
            i = (i + 1) % cnt;
        }
        while (headN != NULL)//if newly created list isn't empty remove first element
            removeFirst(&headN, &tailN);
    }
    fprintf(f, "Average response time = %f \n", (double)totalResponseTime/NUMBER_OF_PROCESSES);
    fprintf(f, "Average turn around time = %f", (double)totalTAT/NUMBER_OF_PROCESSES);
}

//get burst time method created for simplifications
int getBurstTime(struct process * oTemp){ 
    return oTemp->iRemainingBurstTime > TIME_SLICE ? TIME_SLICE : oTemp->iRemainingBurstTime;
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
            if (((struct process *)(ptr -> pData))->iPriority > 
                ((struct process *)(ptr -> pNext -> pData))->iPriority)
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