/*********************/
/* Ryan Evenstad HW7 */
/*********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h> 

struct diskIO{
	struct pcb *item;
	struct diskIO *next;
};
struct diskIO *Dhead, *Dtail;

struct nicIO{
	struct pcb *item;
	struct nicIO *next;
};
struct nicIO *Nhead, *Ntail;

struct keyboardIO{
	struct pcb *item;
	struct keyboardIO *next;
};
struct keyboardIO *Khead, *Ktail;

struct queue{
	struct pcb *me;
	struct queue *next;
};
struct queue *Qhead, *Qtail;

struct pcb{
	char pid[8];
	int priority;
	int credits;
	int stamp; //time_t stamp;
	int quantum;
	int weight;
	int pages[10];
	int numPages;
};

//global stuff for threads
int choice;
struct queue *globalCurrentPCB;
pthread_mutex_t lockM;
pthread_mutex_t lockQ;
//end global stuff for threads

printQueue(){
	printf("IN PRINTQUEUE\n");
	struct queue *temp = Qhead;
	while (temp != NULL)
	{
		printf("%s\t%d\t%d\t%d\t%d -print\n", temp->me->pid, temp->me->priority, temp->me->credits, temp->me->stamp, temp->me->quantum);
		temp = temp->next;
	}
}

//I don't free any other queues...
freeStuff(){
	struct queue *temp = Qhead;
	while (temp != NULL)
	{
		free(temp);
		temp = temp -> next;
	}
}

fillQueue(int max){
	int i;
	char buf[8];
	for (i=0; i<max; i++)
	{
		struct queue *currentQ = (struct queue *)malloc(sizeof(struct queue));
		struct pcb *currentP = (struct pcb *)malloc(sizeof(struct pcb));
		sprintf(buf, "rje%d", i);
		strcpy(currentP -> pid, buf);
		currentP -> priority = (random()%32)+1;
		currentP -> credits = (currentP -> priority)*20;
		currentP -> stamp = 0; //time(NULL);
		//currentP -> quantum = //not used here
		currentP -> weight = random()%2; //0 for lightweight 1 for heavyweight
		int k;
		for (k=0; k<10; k++)
			currentP -> pages[k] = -1;
		int maxPages = (random()%10)+1;
		currentP -> numPages = maxPages;
		
		currentQ -> me = currentP;

		if(Qhead == NULL) //queue is empty
		{
			Qhead = Qtail = currentQ;
			currentQ -> next = NULL;
		}
		else //insert as priority queue
		{
			struct queue *temp = Qhead;
			while (temp != NULL)
			{
				if (currentQ -> me -> credits > temp -> me -> credits) //insert to head
				{
					currentQ -> next = temp;
					Qhead = currentQ;
					break;
				}
				else if (temp -> next == NULL) //add to tail
				{
					currentQ -> next = temp -> next;
					temp -> next = currentQ;
					Qtail = currentQ;
					break;
				}
				else if ((currentQ -> me -> credits <= temp -> me -> credits) && (currentQ -> me -> credits > temp -> next -> me -> credits)) //add to middle
				{
					currentQ -> next = temp -> next;
					temp -> next = currentQ;
					break;
				}
				else
					temp = temp -> next;
			}
		}
	}
}

checkCredits(){
	int hasCredit = 0;
	struct queue *temp = Qhead;
	while (temp != NULL)
	{
		if (temp -> me -> credits > 0)
			hasCredit = 1;
		temp = temp->next;
	}
	if (!hasCredit)
	{
		struct queue *temp = Qhead;
		while (temp != NULL)
		{
			temp -> me -> credits = (temp -> me -> priority)*20;
			temp = temp->next;
		}
	}
}

threadKeyboardQueueManager()
{
	pthread_mutex_lock(&lockQ);
	
	int wait = (random()%9)+1;
	usleep(wait*10000);
	globalCurrentPCB -> me -> credits = globalCurrentPCB -> me -> credits + (globalCurrentPCB -> me -> priority * 100);
	
	pthread_mutex_unlock(&lockQ);
	pthread_exit(0);
}

threadNICQueueManager()
{
	pthread_mutex_lock(&lockQ);
	
	int wait = (random()%90)+10;
	usleep(wait*10000);
	globalCurrentPCB -> me -> credits = globalCurrentPCB -> me -> credits + (globalCurrentPCB -> me -> priority * 70);
	
	pthread_mutex_unlock(&lockQ);
	pthread_exit(0);
}

threadDiskQueueManager()
{
	pthread_mutex_lock(&lockQ);
	
	int wait = (random()%9001)+1000;
	FILE *dataFile;
	dataFile = fopen("int.txt", "r");
	if (dataFile == NULL)
	{
		printf("cannot open file\n");
		exit(0);
	}
	int count = 0;
	char temp[4];
	while (count <= wait)
	{
		fscanf(dataFile, "%s", temp);
		rewind(dataFile);
		count++;
	}
	fclose(dataFile);
	globalCurrentPCB -> me -> credits = globalCurrentPCB -> me -> credits + (globalCurrentPCB -> me -> priority * 20);
	
	pthread_mutex_unlock(&lockQ);
	pthread_exit(0);
}

chooseIO(){
	//printf("IN chooseIO\n");
	
	pthread_t threadQ;
	
	int pickIO = random()%65;
	if (pickIO < 50) //50% keyboard IO
	{
		//int time = random()%10+1; //random number between 1 and 10
		//readFile(time*100);
		//usleep(time);
		//temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 100);
		//return 0;
		pthread_create(&threadQ, NULL, (void*)&threadKeyboardQueueManager, NULL);
		pthread_join(threadQ, NULL);
	}
	else if (pickIO >= 50 && pickIO < 60) //20% NIC IO
	{
		//int time = random()%91+10; //random number between 10 and 100
		//readFile(time*100);
		//usleep(time);
		//temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 70);
		//return 1;
		pthread_create(&threadQ, NULL, (void*)&threadNICQueueManager, NULL);
		pthread_join(threadQ, NULL);
	}
	else if (pickIO >= 60 && pickIO < 65) //10% disk IO
	{
		//int time = random()%9001+1000; //random number between 1000 and 10000
		//readFile(time*100);
		//usleep(time);
		//temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 20);
		//return 2;
		pthread_create(&threadQ, NULL, (void*)&threadDiskQueueManager, NULL);
		pthread_join(threadQ, NULL);
	}
}

lightweightThread(){
	//printf("IN LIGHTWEIGHTTHREAD\n");
	srand(time(NULL));
	int choose = rand()%100;
	if (choose < 5) //terminate
	{
		//printf("Child: 0\n");
		choice = 0;
		//return 0;
	}
	else if (choose >= 5 && choose < 70) //IO
	{
		//printf("Child: 1\n");
		choice = 1;
		//return 1;
	}
	else if (choose >= 70 && choose < 100) //stay alive
	{
		//printf("Child: 2\n");
		choice = 2;
		//return 2;
	}
	pthread_exit(0);
}

threadMemoryManager()
{
	pthread_mutex_lock(&lockM);
	//printf("IN THREADMEMORYMANAGER\n");
	int max = globalCurrentPCB -> me -> numPages;
	int pageIWant = random()%max;
	if (globalCurrentPCB -> me -> pages[pageIWant] == -1) //page is not in memory
	{
		FILE *dataFile;
		dataFile = fopen("pageFaultRead.txt", "r");
		if (dataFile == NULL)
		{
			printf("cannot open file\n");
			exit(0);
		}
		else
		{
			char temp[16];
			fscanf(dataFile, "%s", temp);
			rewind(dataFile);
		}
		fclose(dataFile);
		
		globalCurrentPCB -> me -> pages[pageIWant] = pageIWant;
	}
	pthread_mutex_unlock(&lockM);
	pthread_exit(0);
}

simulate(int max){
	int term = 0, count = max;
	int kill, io, stay;
	char buf[42];
	pid_t childPID;
	struct timeval in, out;
	while(1)
	{
		if (count == 0)
		{
			//printf("ALL PROCESSES ENDED\n");
			break;
		}
		struct queue *temp = Qhead;
		//"pop" from queue
		if (Qhead -> next != NULL)
			Qhead = Qhead -> next;
		temp -> next = NULL;
		temp -> me -> quantum = (random()%90)+10;
		
		globalCurrentPCB = temp;
		gettimeofday(&in, NULL);
		
		pthread_t threadM;
		pthread_create(&threadM, NULL, (void*)&threadMemoryManager, NULL);
		pthread_join(threadM, NULL);
		
		//printf("%s\n", temp -> me -> pid);
		while(1)
		{
			int status;
			kill = 0;
			io = 0;
			stay = 0;
			term = 0;
			if (temp -> me -> credits == 0)
			{
				//printf("OUT OF CREDITS -SIM\n");
				checkCredits();
				break;
			}
			temp -> me -> credits = temp -> me -> credits - 1;
			
			if (temp -> me -> weight == 0) //process is lightweight
			{
				pthread_t threadL;
				pthread_create(&threadL, NULL, (void*)&lightweightThread, NULL);
				pthread_join(threadL, NULL);

				if (choice == 0) //child wants to terminate
					kill = 1;
				else if (choice == 1) //child wants IO
					io = 1;
				else if (choice == 2) //child want to stay alive
					stay = 1;
			}
			
			else if (temp -> me -> weight == 1) //process is heavyweight
			{
				//printf("HEAVYWEIGHT\n");
				childPID = fork();
				if(childPID >= 0) // fork was successful
				{
					if(childPID == 0) // child process
					{
						pid_t myPID = getpid();
						//this needs to be hard coded as far as i can tell
						execlp("/home/revenstad/CS451/HW6/child", "child", NULL);
						//execlp("glxgears", "glxgears", NULL); //from old assignment
					}
					else
					{
						waitpid(childPID, &status, 0);
						//printf("status: %d\n", status);
						if (status == 0) //child want to terminate
							kill = 1;
						else if (status == 256) //child wants IO
							io = 1;
						else if (status == 512) //child want to stay alive
							stay = 1;
					}
				}
			}
			//printf("kill is: %d __ io is: %d __ stay is: %d __ term is: %d\n", kill, io, stay, term);
			if (io != 1)
			{
				int expire = random()%10;
				if ((expire == 0) || (kill == 1)) //process is ended (10% of the time) or child wants to terminate
				{
					gettimeofday(&out, NULL);
					//printf("out %ld\n", out.tv_usec);
					if (out.tv_usec > in.tv_usec)
					{
						int totalTime = out.tv_usec - in.tv_usec;
						temp -> me -> stamp += totalTime;
						//printf("TOTALTIME IS %d and stamp is %d\n", totalTime, temp->me->stamp);
					}
					
					printf("%s\t%d\t%d\n", temp -> me -> pid, temp -> me -> priority, temp -> me -> stamp);
					//printf("TERMINATED -SIM\n");
					term = 1;
					count--;
					
					usleep(500000);
					//sprintf(buf, "kill %d", childPID);
					//system(buf);
					//wait(NULL);
				}
				break;
			}
			else
			{
				/*int pick = chooseIO();
				if (pick == 0) //child want to terminate
					temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 100);
				else if (pick == 1) //child wants IO
					temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 70);
				else if (pick == 2) //child want to stay alive
					temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 20);*/
				chooseIO();
			}
		}
		
		//insert back into queue
		if (term == 0)
		{
			gettimeofday(&out, NULL);
			//printf(">>out %ld\n", out.tv_usec);
			if (out.tv_usec > in.tv_usec)
			{
				int totalTime = out.tv_usec - in.tv_usec;
				temp -> me -> stamp += totalTime;
				//printf("TOTALTIME IS %d and stamp is %d\n", totalTime, temp->me->stamp);
			}
			
			usleep(500000);
			//sprintf(buf, "kill %d", childPID);
			//system(buf);
			//wait(NULL);
			
			if(Qhead == NULL) //queue is empty
			{
				Qhead = Qtail = temp;
				temp -> next = NULL;
			}
			else //insert as priority queue
			{
				struct queue *temp2 = Qhead;
				while (temp2 != NULL)
				{
					if (temp -> me -> credits > temp2 -> me -> credits) //insert to head
					{
						temp -> next = temp2;
						Qhead = temp;
						break;
					}
					else if (temp2 -> next == NULL) //add to tail
					{
						temp -> next = temp2 -> next;
						temp2 -> next = temp;
						Qtail = temp;
						break;
					}
					else if ((temp -> me -> credits <= temp2 -> me -> credits) && (temp -> me -> credits > temp2 -> next -> me -> credits)) //add to middle
					{
						temp -> next = temp2 -> next;
						temp2 -> next = temp;
						break;
					}
					else
						temp2 = temp2 -> next;
				}
			}
		}
	}
}

createFiles()
{
	FILE *file1;
	file1 = fopen("int.txt", "w");
	fprintf(file1, "1");
	fclose(file1);
	
	FILE *file2;
	file2 = fopen("pageFaultRead.txt", "w");
	fprintf(file2, "pageFault");
	fclose(file2);
}

deleteFiles()
{
	remove("int.txt");
	remove("pageFaultRead.txt");
}

int main(){
	time_t t1, t2;
	double t0;
	time(&t1);
	
	FILE *dataFile;
	dataFile = fopen("cs451.conf", "r");
	if (dataFile == NULL)
	{
		printf("cannot open file\n");
		exit(0);
	}
	char temp[128];
	char item[128];
	char maxProcesses[42], timeQuantum[42];
	while(fscanf(dataFile, "%s", temp) != EOF)
	{
		if (strstr(temp, "processes:") != NULL)
		{
			fscanf(dataFile, "%s", maxProcesses);
		}
		else if (strstr(temp, "quantum:") != NULL)
		{
			fscanf(dataFile, "%s", timeQuantum);
		}
		else if (strstr(temp, ":") != NULL)
		{
			fscanf(dataFile, "%s", item);
		}
	}
	fclose(dataFile);
	
	createFiles();
	
	fillQueue(atoi(maxProcesses));
	//printQueue();
	simulate(atoi(maxProcesses));
	freeStuff();
	
	deleteFiles();
	
	time(&t2);
	t0 = difftime(t2, t1);
	printf("Program run-time: %.0f seconds\n", t0);
}