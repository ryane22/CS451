#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
	sem_t *semaphore;
	semaphore = sem_open("/RESEMAPHORE", O_CREAT);
	sem_t *shared;
	shared = sem_open("/RESHARED", O_CREAT);
	
	int semval;
	int shareval;
	
	while(1) {
		sem_getvalue(semaphore, &semval);
		//printf("SEMVAL is %d in %s\n", semval, argv[0]);
		if (semval > 1) {
			//printf("Child killed\n");
			exit(0);
		}
		else if (semval == 1) {
			sem_wait(semaphore);
			sem_getvalue(semaphore, &semval);
			//printf(">>locking  new semval %d  in %s\n", semval, argv[0]);
			
			//printf("BEGIN %s\n", argv[0]);
			//gets the id and the file name
			char arg[42];
			strcpy(arg, argv[0]);
			
			char buf[4] = "";
			int i;
			for (i=0; i<1; i++) {
				sprintf(buf, "%s%c", buf, arg[i]);
			}
			char name[4];
			strcpy(name, buf);
			//printf("name %s", name);
			
			char buf2[42] = "";
			for (++i; i<sizeof(arg); i++) {
				if (arg[i] == '\0')
					break;
				sprintf(buf2, "%s%c", buf2, arg[i]);
			}
			char file[40];
			strcpy(file, buf2);
			//printf("file %s\n", file);
			
			//jump to shared
			int semval;
			sem_getvalue(shared, &semval);
			//printf("semval from child is %d\n", semval);
			FILE *dFile;
			dFile = fopen(file, "r");
			int seek;
			for (seek = 0; seek<semval; seek++)
				fgetc(dFile);
			
			//create a random number of chacters [1-10] to read from a file
			srand(atoi(name));
			int numChar = (rand()%10)+1;
			//printf("\nnumChar is %d\n\n", numChar);
			
			//for loop to sem_post that many times
			int j;
			for (j=0; j<numChar; j++)
				sem_post(shared);
			
			//read from file n number of characters
			char fileBuf[14] = "";
			sprintf(fileBuf, "_p%s", name);                          //add this back later
			int k;
			for (k=0; k<numChar; k++) {
				char c = fgetc(dFile);
				if (c == EOF)
					sprintf(fileBuf, "%s`", fileBuf);
				else
					sprintf(fileBuf, "%s%c", fileBuf, c);
			}
			fclose(dFile);
			
			//write to pipe
			int pipe;
			pipe = open("REPIPE", O_WRONLY);
			write(pipe, fileBuf, sizeof(fileBuf));
			close(pipe);
			//usleep(500);
		}
	}
}