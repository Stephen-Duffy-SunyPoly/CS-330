//Author: Stephen Duffy duffysd
//HW10 IPC
//CS330 2pm TR

#include <fcntl.h>		//For any 'O_' constants
#include <semaphore.h>		//For semaphore stuff	(Link with -pthread)?
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>		//For mode constants
#include <sys/types.h>		//For fork()
#include <time.h>		//For nanosleep()
#include <unistd.h>		//For fork()
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

#define NumForks 4

#define MAX_SIZE 20		//Max size of message in bytes
#define MY_SEM "/duffysd2-10IPCsemN"
#define Q_CAPACITY 10		//How many messages can fit in queue
#define Q_NAME "/duffysd"

int comPipe[2];

//cant find a good way to convert an int to string online so making my own
//returns the number of chars converted
int intToStr(char * str,long value){
	if(value ==0){//if the passed in value was zero assume at the end of recursion and execute the stopping considhen
		str[0]='0';
		str[1]='\0';
		return -1;
	}
	long rem = value%10;//get the last decimal digit
	int index = intToStr(str,value/10)+1;//calculate the pervous char then add 1 to its index
	str[index] = '0'+rem; // convert the digit into the corresponding char
	str[index+1] = '\0';//terminate the string in case the this is the last recursive step;
	return index;//return the current length of the string
}

void producer(char * inputFileName){
	printf("start produce\n");
	FILE * inputFile = fopen(inputFileName,"r");
	if(inputFile == NULL){
		printf("FILE NOT FOUND %s\n",inputFileName);
		return;
	}
	printf("work opend\n");

	pid_t pid = getpid();
	char tmpstr[200];
	intToStr(tmpstr,pid);//convert the pid into a string
	strcat(tmpstr,"_producer.txt");//concatinate
	FILE * logFile = fopen(tmpstr,"w");
	printf("log file readdy\n");

	char semNames[2][NumForks][50];
	//generate the name of each sem and put it in this array for eazy access later
	for(int i=0;i<NumForks;i++){
		strcpy(semNames[0][i],MY_SEM);
		intToStr(semNames[0][i]+strlen(semNames[0][i]),i);
		strcpy(semNames[1][i],semNames[0][i]);
		strcat(semNames[0][i],"a");
		strcat(semNames[1][i],"b");
	}

	printf("sem name Readdy\n");
	//Surprise its later
	sem_t * semIds[2][NumForks];

	//open the semaphores
	for(int j=0;j<2;j++){
		for(int i=0;i<NumForks;i++){
			semIds[j][i] = sem_open(semNames[j][i], O_CREAT, 0600, 0);


			int tmpv;
			sem_getvalue(semIds[j][i],&tmpv);
			printf("created sem %d %d %p value: %d\n",j,i,semIds[j][i],tmpv);
			//Check if semaphore opened
			if(semIds[j][i] == SEM_FAILED){
				printf("producer Sem Failure %d\n",i);
				return;
			}
		}
	}

	printf("sems readdy\n");
	char fileContent[MAX_SIZE];

	printf("Producer starting work\n\n");

	//while there is more "work" to be done
	///*
	int FileReadStatus=0;
	FileReadStatus = fscanf(inputFile, "%s", fileContent);
	while(FileReadStatus != EOF){
		//iterate through all of the children to check if any of them are ready to receive
		for(int i=0;i<NumForks;i++){
			int val,suc;
			sem_getvalue(semIds[0][i],&val);
			suc = sem_trywait(semIds[0][i]);
			//printf("Attempting to check %d value: %d success: %d\n",i,val,suc);
			if(suc>=0){
				while(sem_trywait(semIds[0][i])==0){}//make sure that the sem is set to 0
				char sendString[100];
				strcpy(sendString,fileContent);
				size_t inputLength = strlen(fileContent);
				//add the seperator
				sendString[inputLength] = ',';
				sendString[inputLength+1] = '\0';

				long exeTime = ((random() % 30000000000) + 1000000000);	//should be between 0.5 and 1.5 seconds)
				//add the exe time to the sending content
				intToStr(sendString+inputLength+1,exeTime);
				//conviently the number to string conversion function i wrote is perfect for this application

				write(comPipe[1],sendString,strlen(sendString));

				printf("sending string: %s length: %ld to: %d\n",sendString,strlen(sendString),i);
				fprintf(logFile,"%s\n",sendString);

				sem_post(semIds[1][i]);//signal to the consumer to read from the pipe

				sem_wait(semIds[0][i]);//wait for the consumor to have read from the pipe
				/*struct timespec sleepTime;

				sleepTime.tv_sec =0;
				sleepTime.tv_nsec = 500000000;
				nanosleep(&sleepTime,NULL);*/
				//read the next line from the file
				FileReadStatus = fscanf(inputFile, "%s", fileContent);
				if(FileReadStatus == EOF){
					break;
				}
			}
		}

	}
//*/
	for(int j=0;j<2;j++){
		for(int i=0;i<NumForks;i++){
			//reset the value of the sems in case they are not all currently 0
			while(sem_trywait(semIds[j][i])==0){}
		}
	}

	fclose(logFile);
	fclose(inputFile);
}

void consumer(int childId){
	pid_t pid = getpid();
	char tmpstr[200];
	intToStr(tmpstr,pid);//convert the pid into a string
	strcat(tmpstr,"_consumer.txt");//concatinate
	FILE * logFile = fopen(tmpstr,"w");

	//make the buffer 1  bigger then the max returnable size so that an extra null byte can be appended to the end if the entire buffre is used
	char fileContents[MAX_SIZE+1];
	//initilise the entire buffer to :
	for(int i=0;i<MAX_SIZE+1;i++){
		fileContents[i]=':';
	}

	int post;
	sem_t * readReadySemID,* pipeReadySemID;


	char semName[50],otherSemName[50];
	strcpy(semName,MY_SEM);//create the sem name
	intToStr(semName+strlen(semName),childId);
	strcpy(otherSemName,semName);
	//make a name for both the read and wright ops
	strcat(semName,"a");
	strcat(otherSemName,"b");

	readReadySemID = sem_open(semName, O_CREAT, 0600, 0);
	pipeReadySemID = sem_open(otherSemName, O_CREAT, 0600, 0);
	if(readReadySemID == SEM_FAILED || pipeReadySemID == SEM_FAILED) {
		perror("child sem_open");
		return;
	}

	post = sem_post(readReadySemID);//signal to the producer that this consumer is ready
	while(1){
		//wait for the producer to put data into the pipe
		sem_wait(pipeReadySemID);
		//read from the pipe
		ssize_t readLength = read(comPipe[0], fileContents, MAX_SIZE);
		fileContents[readLength]='\0';//terminate the received string because that is not automatically done

		sem_post(readReadySemID);//signal to the producer that the pipe had been read from

		//save the data to the log file
		struct timeval now;
		gettimeofday(&now,NULL);
		printf("Consumer %d: recieved. [%s] length: %ld\n",childId,fileContents,strlen(fileContents));
		fprintf(logFile,"%s %ld %ld\n",fileContents,now.tv_sec,now.tv_usec);

		int comaaPos=0,inLength = strlen(fileContents);
		for(int i=0;i<inLength;i++){
			if(fileContents[i]==','){
				comaaPos=i+1;
				break;
			}
		}
		long long exeTime = strtol(fileContents+comaaPos,NULL,10);

		fflush(logFile);//flush the log file to make sure it gets saved

		//sleep to simulate the work
		struct timespec sleepTime;
		sleepTime.tv_sec = exeTime / 1000000000;
		sleepTime.tv_nsec = exeTime % 1000000000;
		//printf("con wait start %d\n",childId);
		//printf("sleeping for %ld secconds and %ld ns\n",sleepTime.tv_sec,sleepTime.tv_nsec);
		nanosleep(&sleepTime,NULL);
		//printf("con wait end %d\n",childId);

		//unlock the sem
		post = sem_post(readReadySemID);//signal to the producer that this consumer is ready
	}
	//can't really get to here as it will be killed inside of the loop but just in case
	fclose(logFile);
}

int main(void) {
	if(pipe(comPipe)==-1){
		perror("pipe"); // error in pipe
		return EXIT_FAILURE;
	}
	//create all the forks;
	pid_t forkPids[NumForks];
	for(int i=0;i<NumForks;i++){
		forkPids[i] = fork();
		if(forkPids[i] == 0){
			consumer(i);
			return EXIT_SUCCESS;
		}
	}
	//do the producer
	producer("fakeWork.txt");
	//kill the forks

	struct timespec sleepTime;
	sleepTime.tv_sec = 5;
	sleepTime.tv_nsec = 500;
	nanosleep(&sleepTime,NULL);
	for(int i=0;i<NumForks;i++){
		kill(forkPids[i],9);
	}
	return EXIT_SUCCESS;
}
