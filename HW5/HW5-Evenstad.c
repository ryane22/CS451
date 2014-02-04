/*********************/
/* Ryan Evenstad HW5 */
/*********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

/*struct diskIO{
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
struct keyboardIO *Khead, *Ktail;*/

struct queue{
	struct pcb *me;
	struct queue *next;
};
struct queue *Qhead, *Qtail;

struct pcb{
	char pid[8];
	int priority;
	int credits;
	time_t stamp;
	int quantum;
};

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
		currentP -> stamp = time(NULL);
		//currentP -> quantum = //not used in this program
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
	int hasCredit;
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
		}
	}
}

simulate(int max){
	int term = 0, count = max;
	char buf[42];
	pid_t childPID;
	while(1)
	{
		term = 0;
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
		//printf("%s\n", temp -> me -> pid);
		while(1)
		{
			term = 0;
			if (temp -> me -> credits == 0)
			{
				//printf("OUT OF CREDITS -SIM\n");
				checkCredits();
				break;
			}
			
			childPID = fork();
			if(childPID >= 0) // fork was successful
			{
				if(childPID == 0) // child process
				{
					printf("IN CHILD");
					pid_t myPID = getpid();
					execlp("glxgears", "glxgears", NULL);
				}
			}
			
			temp -> me -> credits = temp -> me -> credits - 1;
			int expire = random()%10;
			if (expire == 0) //process is ended (10% of the time)
			{
				time_t now = time(NULL);
				printf("%s\t%d\t%d\n", temp -> me -> pid, temp -> me -> priority, difftime(temp -> me -> stamp, now));
				//printf("TERMINATED -SIM\n");
				term = 1;
				count--;
				
				usleep(500000);
				sprintf(buf, "kill %d", childPID);
				system(buf);
				wait(NULL);
				break;
			}
			else
				break;
		}
		//kill and insert back into queue
		if (term != 1) //if process is not ended (90% of the time)
		{
			int pickIO = random()%10;
			if (pickIO <= 6) //70% keyboard IO
			{
				int time = random()%10+1; //random number between 1 and 10
				usleep(time);
				temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 100);
			}
			else if (pickIO <= 8) //20% NIC IO
			{
				int time = random()%91+10; //random number between 10 and 100
				usleep(time);
				temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 70);
			}
			else if (pickIO == 9) //10% disk IO
			{
				int time = random()%9001+1000; //random number between 1000 and 10000
				usleep(time);
				temp -> me -> credits = temp -> me -> credits + (temp -> me -> priority * 20);
			}
			
			usleep(500000);
			sprintf(buf, "kill %d", childPID);
			system(buf);
			wait(NULL);
			
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

int main(){
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
			//enqueue item
			//enqueue(atoi(item)); 
			//printf("%s\n", item);
		}
	}
	fclose(dataFile);
	
	fillQueue(atoi(maxProcesses));
	//printQueue();
	simulate(atoi(maxProcesses));
	freeStuff();
}