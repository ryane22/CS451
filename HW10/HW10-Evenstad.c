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
	sem_t *shared;
	
	if (argc < 2) {
		printf("please enter file name as command line argument\n");
		exit(1);
	}
	//check if argv[1] is a valid filename
	FILE *dataFile;
	dataFile = fopen(argv[1], "r");
	if (dataFile == NULL) {
		printf("cannot open file\n");
		exit(1);
	}
	fclose(dataFile);
	
	printf("\n\n Please wait...\n\n");
	
	//open outfile
	FILE *outfile;
	outfile = fopen("hw10.out", "w");
	fprintf(outfile, "");
	fclose(outfile);
	
	//create semaphore
	semaphore = sem_open("/RESEMAPHORE", O_CREAT, 0600, 0);
	sem_post(semaphore);
	
	//create shared memory
	int shareval = 0;
	shared = sem_open("/RESHARED", O_CREAT, 0600, 0);
	
	//create pipe
	char maxPipe[10];
	int pipe;
	pipe = mkfifo("REPIPE", 0600);
	
	//fork children
	int i;
	char buf[42];
	for (i = 0; i < 5; i++) {
		int childPID = fork();
		if(childPID >= 0) { // fork was successful
			if(childPID == 0) { // child process
				sprintf(buf, "%d %s", i+1, argv[1]);
				//printf("%s\n", buf);
				pid_t myPID = getpid();
				execlp("/home/revenstad/CS451/HW10/p", buf, NULL);
			}
		}
	}
	
	//sleep(1);
	int semval;
	while(1) {
		sem_getvalue(semaphore, &semval);
		if (semval == 0) {
			//read from pipe
			char bufP[14] = "";
			pipe = open("REPIPE", O_RDONLY);
			read(pipe, bufP, sizeof(bufP));
			close(pipe);
			//printf("\nbufP is %s\n\n", bufP);
			
			if (strstr(bufP, "`")>0) {
				int temp;
				sem_post(semaphore);
				sem_post(semaphore);
				sem_post(semaphore);
				sem_getvalue(semaphore, &temp);
				char bufF[14]="";
				int m;
				for (m=0; m<14; m++) {
					if (bufP[m] == '`')
						break;
					else
						sprintf(bufF, "%s%c", bufF, bufP[m]);
				}
				outfile = fopen("hw10.out", "a");
				fprintf(outfile, "%s", bufF);
				sem_getvalue(shared, &shareval);
				fprintf(outfile, "\n\nShared memory value: %d", shareval);
				fclose(outfile);
				
				break;
			}
			//open a file to write to
			outfile = fopen("hw10.out", "a");
			fprintf(outfile, "%s", bufP);
			fclose(outfile);
			
			//usleep(5000);
			sem_post(semaphore);
		}
	}
	
	sleep(1);
	
	printf("Done");
	printf("\n\n");
	
	sem_close(semaphore);
	sem_unlink("/RESEMAPHORE");
	sem_close(shared);
	sem_unlink("/RESHARED");
	
	unlink("REPIPE");
	
	exit(0);
}