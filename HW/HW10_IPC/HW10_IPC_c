//Author: Stephen Duffy duffysd
//HW10 IPC
//CS330 2pm TR

#include <fcntl.h>		//For any 'O_' constants
#include <mqueue.h>		//For message queues	(Link with -lrt)
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
	//Create message queue
	struct mq_attr mqAttr;

	mqAttr.mq_flags = 0;		//Ignored in mq_open()
	mqAttr.mq_maxmsg = Q_CAPACITY;
	mqAttr.mq_msgsize = MAX_SIZE;
	mqAttr.mq_curmsgs = 0;		//Also ignored in mq_open()

	long wait =0;
	//open the messgae queue
	mqd_t jobQ = mq_open(Q_NAME, O_CREAT | O_WRONLY, 0644, &mqAttr);
	if((mqd_t)-1 == jobQ){
		//somethign went wrong. it might not exsist lets try and make it;
		printf("producer queue error\n");
		return;
	}
	printf("mq readdy\n");
	char semNames[NumForks][50];
	//generate the name of each sem and put it in this array for eazy access later
	for(int i=0;i<NumForks;i++){
		strcpy(semNames[i],MY_SEM);
		intToStr(semNames[i]+strlen(semNames[i]),i);
	}
	printf("sem name Readdy\n");
	//Surprise its later
	sem_t * semIds[NumForks];
	//open the semaphores
	for(int i=0;i<NumForks;i++){
		semIds[i] = sem_open(semNames[i], O_CREAT, 0600, 0);
		printf("created sem %d %p\n",i,semIds[i]);
		//Check if semaphore opened
		if(semIds[i] == SEM_FAILED){
			printf("producer Sem Failure %d\n",i);
			return;
		}
	}
	printf("sems readdy\n");
	char fileContent[MAX_SIZE];


	printf("Producer starting work\n");


	//while there is more "work" to be done
	///*
	while(fscanf(inputFile, "%s", fileContent) != EOF){
		//iterate through all of the children to check if any of them are ready to receive
		for(int i=0;i<NumForks;i++){
			if(sem_trywait(semIds[i])>=0){
				char sendString[100];
				strcpy(sendString,fileContent);
				size_t inputLength = strlen(fileContent);
				//add the seperator
				sendString[inputLength] = ',';
				sendString[inputLength+1] = '\0';

				long exeTime = ((random() % 1000000000) + 500000000);	//should be between 0.5 and 1.5 seconds)
				//add the exe time to the sending content
				intToStr(sendString+inputLength+1,exeTime);
				//conviently the number to string conversion funtion i wrote is perfect for this application

				int sendinfo = mq_send(jobQ, sendString, strlen(sendString),0);
				if(sendinfo == -1)
				{
					printf("sending Error\n");
					return;
				}

				printf("sending string: %s length: %ld\n",sendString,strlen(sendString));
				fprintf(logFile,"%s\n",sendString);

				struct timespec sleepTime;
				int st = (random() % 1000000000) + 500000000;
				sleepTime.tv_sec = st / 1000000000;
				sleepTime.tv_nsec = st % 1000000000;
				nanosleep(&sleepTime,NULL);
				break;
			}
		}

	}
//*/
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
	mqd_t jobQ;
	sem_t * semID;

	char semName[50];
	strcpy(semName,MY_SEM);//create the sem name
	intToStr(semName+strlen(semName),childId);

	semID=sem_open(semName, O_CREAT, 0600, 0);
	if(semID == SEM_FAILED) {
		perror("child sem_open");
		return;
	}

	struct mq_attr mqAttr;
	mqAttr.mq_flags = 0;		//Ignored in mq_open()
	mqAttr.mq_maxmsg = Q_CAPACITY;
	mqAttr.mq_msgsize = MAX_SIZE;
	mqAttr.mq_curmsgs = 0;		//Also ignored in mq_open()

	//create the message reader
	jobQ = mq_open(Q_NAME, O_CREAT | O_RDONLY, 0644, &mqAttr);
	if((mqd_t)-1 == jobQ){
		//somethign went wrong. it might not exsist lets try and make it;
		printf("consumer queue error\n");
		exit(EXIT_FAILURE);
	}

	post = sem_post(semID);//signal to the producer that this consumer is ready
	while(1){
		//read from the queue
		ssize_t readLength = mq_receive(jobQ, fileContents, MAX_SIZE, NULL);
		fileContents[readLength]='\0';//terminate the received string because that is not automatically done

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
		int exeTime = strtol(fileContents+comaaPos,NULL,10);

		fflush(logFile);//flush the log file to make sure it gets saved

		struct timespec sleepTime;
		sleepTime.tv_sec = exeTime / 1000000000;
		sleepTime.tv_nsec = exeTime % 1000000000;
		nanosleep(&sleepTime,NULL);

		//unlock the sem
		post = sem_post(semID);//signal to the producer that this consumer is ready
	}
	//can't really get to here as it will be killed inside of the loop but just in case
	fclose(logFile);
}

int main(void) {
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
