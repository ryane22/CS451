#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//global variables for threads
char d1,d2,d3,d4,d5;
int e1,e2,e3,e4,e5;

drive1Manage(){
	FILE *dataFile;
	dataFile = fopen("drive1.data", "r");
	if (dataFile == NULL)
	{
		printf("cannot open drive1.data\n");
		exit(0);
	}
	else
	{
		char temp;
		while((temp=fgetc(dataFile)) != EOF)
		{
			if(d1 == '~') //set d1 to current character in the file
				d1 = temp;
			else
			{
				//printf("REWIND BY ONE\n");
				fseek(dataFile, -1, SEEK_CUR);
			}
		}
		//printf("EOF HAPPENED\n");
		usleep(1000);
		e1=1;
		pthread_exit(0);
	}
}
drive2Manage(){
	FILE *dataFile;
	dataFile = fopen("drive2.data", "r");
	if (dataFile == NULL)
	{
		printf("cannot open drive2.data\n");
		exit(0);
	}
	else
	{
		char temp;
		while((temp=fgetc(dataFile)) != EOF)
		{
			if(d2 == '~')
				d2 = temp;
			else
			{
				fseek(dataFile, -1, SEEK_CUR);
			}
		}
		usleep(1000);
		e2=1;
		pthread_exit(0);
	}
}
drive3Manage(){
	FILE *dataFile;
	dataFile = fopen("drive3.data", "r");
	if (dataFile == NULL)
	{
		printf("cannot open drive3.data\n");
		exit(0);
	}
	else
	{
		char temp;
		while((temp=fgetc(dataFile)) != EOF)
		{
			if(d3 == '~')
				d3 = temp;
			else
			{
				fseek(dataFile, -1, SEEK_CUR);
			}
		}
		usleep(1000);
		e3=1;
		pthread_exit(0);
	}
}
drive4Manage(){
	FILE *dataFile;
	dataFile = fopen("drive4.data", "r");
	if (dataFile == NULL)
	{
		printf("cannot open drive4.data\n");
		exit(0);
	}
	else
	{
		char temp;
		while((temp=fgetc(dataFile)) != EOF)
		{
			if(d4 == '~')
				d4 = temp;
			else
			{
				fseek(dataFile, -1, SEEK_CUR);
			}
		}
		usleep(1000);
		e4=1;
		pthread_exit(0);
	}
}
drive5Manage(){
	FILE *dataFile;
	dataFile = fopen("drive5.data", "r");
	if (dataFile == NULL)
	{
		printf("cannot open drive5.data\n");
		exit(0);
	}
	else
	{
		char temp;
		while((temp=fgetc(dataFile)) != EOF)
		{
			if(d5 == '~')
				d5 = temp;
			else
			{
				fseek(dataFile, -1, SEEK_CUR);
			}
		}
		usleep(1000);
		e5=1;
		pthread_exit(0);
	}
}

getData(){
	while(e1!=1||e2!=1||e3!=1||e4!=1||e5!=1)
	{
		while (e1!=1)
		{
			if(d1=='~')
				continue;
			printf("%c",d1);
			d1='~';
			break;
		}
		while (e2!=1)
		{
			if (d2=='~')
				continue;
			printf("%c",d2);
			d2='~';
			break;
		}
		while (e3!=1)
		{
			if (d3=='~')
				continue;
			printf("%c",d3);
			d3='~';
			break;
		}
		while (e4!=1)
		{
			if (d4=='~')
				continue;
			printf("%c",d4);
			d4='~';
			break;
		}
		while (e5!=1)
		{
			if (d5=='~')
				continue;
			printf("%c",d5);
			d5='~';
			break;
		}
	}
	printf("\n");
}

int main(){
	d1=d2=d3=d4=d5='~';
	e1=e2=e3=e4=e5=0;
	
	pthread_t drive1Thread, drive2Thread, drive3Thread, drive4Thread, drive5Thread;
	
	pthread_create(&drive1Thread, NULL, (void*)&drive1Manage, NULL);
	pthread_detach(drive1Thread);
	
	pthread_create(&drive2Thread, NULL, (void*)&drive2Manage, NULL);
	pthread_detach(drive2Thread);
	
	pthread_create(&drive3Thread, NULL, (void*)&drive3Manage, NULL);
	pthread_detach(drive3Thread);
	
	pthread_create(&drive4Thread, NULL, (void*)&drive4Manage, NULL);
	pthread_detach(drive4Thread);
	
	pthread_create(&drive5Thread, NULL, (void*)&drive5Manage, NULL);
	pthread_detach(drive5Thread);
	
	getData();
}