/*********************/
/* Ryan Evenstad HW4 */
/*********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

struct queue{
	//int time;  //will likely not be needed with this being a queue of pcb structs
	struct pcb *me;
	struct queue *next;
};
struct queue *head, *tail;

struct pcb{
	char pid[8];
	int priority;
	int credits;
	time_t stamp;
	int quantum;
};

printQueue(){
	printf("IN PRINTQUEUE\n");
	struct queue *temp = head;
	while (temp != NULL)
	{
		printf("%s\t%d\t%d\t%d\t%d -print\n", temp->me->pid, temp->me->priority, temp->me->credits, temp->me->stamp, temp->me->quantum);
		temp = temp->next;
	}
}

freeStuff(){
	struct queue *temp = head;
	while (temp != NULL)
	{
		free(temp);
		temp = temp -> next;
	}
}

/*enqueue(int element){
	//printf("%d -enqueue\n", element);
	struct queue *current = (struct queue *)malloc(sizeof(struct queue));
	current -> time = element;
	if(head == NULL) //queue is empty
	{
		head = tail = current;
		current -> next = NULL;
	}
	else
	{   //insert to tail
		tail -> next = current;
		tail = current;
	}
}*/

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
		
		if(head == NULL) //queue is empty
		{
			head = tail = currentQ;
			currentQ -> next = NULL;
		}
		else //insert to tail
		{
			tail -> next = currentQ;
			tail = currentQ;
		}
	}
}

checkCredits(){
	int hasCredit;
	struct queue *temp = head;
	while (temp != NULL)
	{
		if (temp -> me -> credits > 0)
			hasCredit = 1;
		temp = temp->next;
	}
	if (!hasCredit)
	{
		struct queue *temp = head;
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
		//if (head == NULL)
		if (count == 0)
		{
			//printf("ALL PROCESSES ENDED\n");
			break;
		}
		struct queue *temp = head;
		//"pop" from queue
		if (head -> next != NULL)
			head = head -> next;
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
			
			//new stuff
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
			/*else //parent process
			{
				printf("Child's PID is %d\n", childPID);
				wait(childPID, NULL, 0);
			}
			//end new stuff*/
			
			temp -> me -> credits = temp -> me -> credits - 1;
			int expire = random()%10;
			if (expire == 0)
			{
				time_t now = time(NULL);
				printf("%s\t%d\t%d\n", temp -> me -> pid, temp -> me -> priority, difftime(temp -> me -> stamp, now));
				//printf("TERMINATED -SIM\n");
				term = 1;
				count--;
				
				sleep(1);
				sprintf(buf, "kill %d", childPID);
				system(buf);
				wait(NULL);
				break;
			}
			else
				break;
		}
		//kill and insert back into queue
		if (term != 1)
		{
			sleep(1);
			sprintf(buf, "kill %d", childPID);
			system(buf);
			wait(NULL);
			
			tail -> next = temp;
			tail = temp;
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