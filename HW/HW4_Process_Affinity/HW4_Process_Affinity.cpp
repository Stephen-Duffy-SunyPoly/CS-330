//Author: Stephen Duffy duffysd
//HW4 Process Affinity
//CS330 2pm TR

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <cstdbool>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

typedef class Timer{
	public:
		//prototype for required functions
		void startTimer();
		void endTimer();
		void printTimer();
	private:
		struct timeval startime;
		struct timeval endtime;
} Timer;

int main() {
	int numForks = 8;

	ifstream filein("input.txt");//the input file
	char c1;
	cout << "reading in data"<< endl;
	Timer readTime;
	readTime.startTimer();
	stringstream ss;
	//read the entire file in 1 char at a time
	while(filein >> c1){
		ss << c1;
	}
	string inputData;
	ss >> inputData;//get the file content as a string
	readTime.endTimer();
	cout << "finished reading data" << endl;
	readTime.printTimer();

	int perProcWorkLoad = inputData.length()/numForks;

	const char searchForChar = 'k';
	Timer t1,t2;
	t1.startTimer();

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(0,&mask);

	for(int i=0;i<numForks;i++){
		if(fork() ==0){
			//set affinity here
			sched_setaffinity(0,sizeof(cpu_set_t),&mask);

			volatile int num = 0;//the number of chars found
			int limit = perProcWorkLoad*(i+1);//were to search to in the string
			if(i==numForks-1){//if this is the last fork
				limit=inputData.length();//set the limit to the end of the string
			}
			//search through the seaction of the string
			for(int j=perProcWorkLoad*i;j<limit;j++){
				if(inputData[j]==searchForChar){//if the current char is the special char
					num++;//increment the count
				}
			}
			exit(num);
		}


	}
	int total = 0;
	int output =0;
	while(wait(&output) != -1){
		total+=output;
	}
	t1.endTimer();

	cout << "first attempt found " << total << "* " << searchForChar << endl;
	t1.printTimer();

	//again this time without affinfiy set
	t2.startTimer();
	for(int i=0;i<numForks;i++){
		if(fork() ==0){
			volatile int num = 0;//the number of chars found
			int limit = perProcWorkLoad*(i+1);//were to search to in the string
			if(i==numForks-1){//if this is the last fork
				limit=inputData.length();//set the limit to the end of the string
			}
			//search through the seaction of the string
			for(int j=perProcWorkLoad*i;j<limit;j++){
				if(inputData[j]==searchForChar){//if the current char is the special char
					num++;//increment the count
				}
			}
			exit(num);
		}


	}
	 total = 0;
	 output =0;
	while(wait(&output) != -1){
		total+=output;
	}
	t2.endTimer();
	cout << "second attempt found " << total << "* " << searchForChar << endl;
	t2.printTimer();

	return EXIT_SUCCESS;
}

void Timer::startTimer(){
	//save the current time as the start time
	gettimeofday(&startime,NULL);
}

void Timer::endTimer(){
	//save the current timeas the end time
	gettimeofday(&endtime,NULL);
}

void Timer::printTimer(){
	//convert everything to micro secconds and store it in a 64bit integer to make sure we don't loose any data
	long long totalStartMicroSecconds = startime.tv_usec;
	totalStartMicroSecconds += (long long)startime.tv_sec * 1000000;

	long long totalEndMicroSecconds = endtime.tv_usec;
	totalEndMicroSecconds += (long long)endtime.tv_sec * 1000000;
	//calculate the difference
	long long totalElapsedMicroSecconds = totalEndMicroSecconds - totalStartMicroSecconds;
	//keep the secconds as 64bit incase you want to time something for over 65 years
	long long elapsedSecconds = totalElapsedMicroSecconds / 1000000;
	//micro seconds are bounded at 1000000 so they can say as 32 bit
	long elapsedMicroSecconds = (long)(totalElapsedMicroSecconds % 1000000);

	//print the values
	cout << "Time Elapsed: " << elapsedSecconds << "s " << elapsedMicroSecconds << "us " << endl;
}

