/*********************/
/* Ryan Evenstad HW2 */
/*********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct queue{
	int time;
	struct queue *next;
};
struct queue *head, *tail;

printQueue(){
	printf("IN PRINTQUEUE\n");
	struct queue *temp = head;
	while (temp != NULL)
	{
		printf("%d -print\n", temp->time);
		temp = temp->next;
	}
}

enqueue(int element){
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
	while(fscanf(dataFile, "%s", temp) != EOF)
	{
		if (strstr(temp, ":") != NULL)
		{
			fscanf(dataFile, "%s", item);
			//enqueue item
			enqueue(atoi(item));
			printf("%s\n", item);
		}
	}
	//printQueue();
}