#include <stdio.h>
#include <stdlib.h>

int main(){
	//usleep(1000000); //waits to make it seem like this is doing something
	srand(time(NULL));
	int choose = rand()%3;
	if (choose == 0) //terminate
	{
		//printf("Child: 0\n");
		_Exit(0);
	}
	else if (choose == 1) //IO
	{
		//printf("Child: 1\n");
		_Exit(1);
	}
	else if (choose == 2) //stay alive
	{
		//printf("Child: 2\n");
		_Exit(2);
	}
	else
		printf("WTF??");
}