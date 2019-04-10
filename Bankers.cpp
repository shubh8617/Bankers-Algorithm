//My name is Shubh Sonthalia
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

int r,P;
int *resources;
int **allocated;
int **max;
int **need;
int *safeSeq;
int nProcessran = 0;

pthread_mutex_t lockr;
pthread_cond_t condition;

bool SafeSeq();

void* processCode(void* );

int main() {
	printf("\t\t*************BANKERS ALGORITHM**************\t\t");
        printf("\nEnter Number of processes:");
        scanf("%d", &P);
        printf("\nEnter Number of resources:");
        scanf("%d", &r);
        resources = (int *)malloc(r* sizeof(*resources));
        //taking input of currently avaiilable resources
        printf("\nCurrently Available resources:= ");
        for(int i=0; i<r; i++)
            scanf("%d", &resources[i]);
        //dynamically declaring maximum matrix 
        max = (int **)malloc(P * sizeof(*max));
        for(int i=0; i<P; i++)
        	max[i] = (int *)malloc(r * sizeof(**max));
        //dynamically declaring allocation matrix
        allocated = (int **)malloc(P * sizeof(*allocated));
        for(int i=0; i<P; i++)
        	allocated[i] = (int *)malloc(r * sizeof(**allocated));
        // allocated
        printf("\n");
        system("cls");
        printf("\t\t********Allocation of resources to process***************\n");
        printf("Process\t\tResources\n");
        printf("\t\t");
        for(int i=0;i<r;i++)
        {
        	printf("R%d ",i);
		}
        for(int i=0; i<P; i++) {
                printf("\nP%d------------->", i+1);
                for(int j=0; j<r; j++)
                        scanf("%d", &allocated[i][j]);
        }
        printf("\n");
	// maximum required resources
		printf("\t\t********Maximum Requirement of Processes****************\n");
		printf("Process\t\tResources\n");
        printf("\t\t");
        for(int i=0;i<r;i++)
        {
        	printf("R%d ",i);
		}
        for(int i=0; i<P; i++) {
                printf("\nP%d------------->", i+1);
                for(int j=0; j<r; j++)
                        scanf("%d", &max[i][j]);
        }
        printf("\n");
	// Calculating need matrix
        need = (int **)malloc(P * sizeof(*need));
        for(int i=0; i<P; i++)
                need[i] = (int *)malloc(r * sizeof(**need));

        for(int i=0; i<P; i++)
                for(int j=0; j<r; j++)
                        need[i][j] = max[i][j] - allocated[i][j];
	// for getting safe sequence
	safeSeq = (int *)malloc(P * sizeof(*safeSeq));
        for(int i=0; i<P; i++) safeSeq[i] = -1;
        if(!SafeSeq()) {
                printf("THIS OPERATION WILL LEAD TO AN UNSAFE STATE\n");
                printf("Operation Terminating....\n");
                sleep(1);
                printf("Terminated\n");
                exit(-1);
        }
    	printf("SHOWING DETAILED PROCESS EXECUTION\n");
		printf("Waiting........\n");
		sleep(1);
		printf("Here it Goes\n");
	// creating multi threads for processes
		pthread_t processes[P];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
	int processNumber[P];
	for(int i=0; i<P; i++) processNumber[i] = i;
        for(int i=0; i<P; i++)
                pthread_create(&processes[i], &attr, processCode, (void *)(&processNumber[i]));
        for(int i=0; i<P; i++)
                pthread_join(processes[i], NULL);
        printf("\nAll Processes Finished\n");
        printf("The System Is in Safe State\n");
        printf("The Safe Sequence Generated is\n");
        printf("< ");
		 for(int i=0; i<P; i++) {
                printf("%-3d", safeSeq[i]+1);
        }
		printf(">");		
        free(resources); //releasing resources
        for(int i=0; i<P; i++) {
                free(allocated[i]);
                free(max[i]);
		free(need[i]);
        }
        free(allocated);
        free(max);
		free(need);
        free(safeSeq);
}
bool SafeSeq() { //for generating safe sequence
        int temp[r];
        for(int i=0; i<r; i++) 
		{
		temp[i] = resources[i];
		}
        bool finished[P];
        for(int i=0; i<P; i++) 
		{
		finished[i] = false;
		}
        int notfinished=0;
        while(notfinished < P) {
                bool safe = false;
                for(int i=0; i<P; i++) {
                        if(!finished[i]) {
                                bool possible = true;
                                for(int j=0; j<r; j++)
                                        if(need[i][j] > temp[j]) {
                                                possible = false;
                                                break;
                                        }
                                if(possible) {
                                        for(int j=0; j<r; j++)
                                                temp[j] += allocated[i][j];
                                        safeSeq[notfinished] = i;
                                        finished[i] = true;
                                        ++notfinished;
                                        safe = true;
                                }
                        }
                }
                if(!safe) {
                	int k=0;
                	while(k<P)
                	{
                		safeSeq[k]=-1;
                		k++;
					}   
                        return false; // If safe sequence is not coming
                }
        }
        return true; // If it is coming
}
void* processCode(void *arg) {
        int p = *((int *) arg);
        //resources going to be locked
        pthread_mutex_lock(&lockr);
        while(p != safeSeq[nProcessran])
                pthread_cond_wait(&condition, &lockr);
        printf("\n--> Process %d", p+1);
        printf("\n\tAllocated resource :");
        for(int i=0; i<r; i++)
                printf("%3d", allocated[p][i]);
        printf("\n\tNeed of the Process : ");
        for(int i=0; i<r; i++)
                printf("%3d", need[p][i]);
        printf("\n\tAvailable resources : ");
        for(int i=0; i<r; i++)
                printf("%3d", resources[i]);
        printf("\n"); sleep(1);
        printf("\tResource Allocated!");
        printf("\n"); sleep(1);
        printf("\tProcess Releasing resource...");
        printf("\n"); sleep(1);
        printf("\tResource released!");
        int i=0;
	while(i<r)
	{
    	resources[i] += allocated[p][i];
        i++;}
        printf("\n\tNow Available : ");
        i=0;
		while(i<r)
		{
		printf("%3d", resources[i]);	
		i++;
		}    
        printf("\n\n");
        sleep(1);
        nProcessran++;
        pthread_cond_broadcast(&condition);
        pthread_mutex_unlock(&lockr);
	pthread_exit(NULL);
}
