/*Author: Stephen Duffy duffysd
 *HW 6: Thread Scheduling
 *CS330 2pm TR
*/

#include <iostream>
#include <pthread.h>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#ifndef _WIN32//if not on windows
	#include <sys/types.h>
	#include <sys/wait.h>
#else
	void nice(int n){}//Define nice for windows so it can be compiled
#endif
using namespace std;

//a class to hold all the timerings
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

typedef struct ThreadInfo{
	int startIndex;
	int endIndex;
	char searchFor;
	string * content;
	int numberFound =0;
	int niceVal;
} ThreadInfo;

void * coutThread(void * args);

void processWithForks(string &s,char c,int numForks,int nicenes,int cpf);

int main() {
	cout << "What char do you want to search for? ";
	char searchFor;
	cin >> searchFor;//get the char to search for

	cout << "How many threads do you want to use? ";
	size_t numThreads;
	cin >> numThreads;//get the number of threads

	cout << "Input file name: ";
	string inputFileName;
	cin >> inputFileName;

	cout << "Enter niceness value: ";
	int niceValue=0;
	cin >> niceValue;

	cout << endl << "Searching for char "<<searchFor <<" using " << numThreads << " threads in the file " << inputFileName <<" with a nice value of: "<< niceValue<< endl;

	//timer 2: read file time
	//timer 3: search time
	Timer timer2,timer3;
	timer2.startTimer();

	//read in the file
	ifstream fileIn(inputFileName);
	stringstream ss;
	char readIn;
	while(fileIn >> readIn){
		ss << readIn;
	}
	//this usually takes longer then the searching
	string fileContent = ss.str();
	timer2.endTimer();

	cout << "File read time:"<< endl;
	timer2.printTimer();

	cout << "processing File Using Threads:"<<endl;
	//create a place to store threads and their data
	pthread_t threads[numThreads];
	ThreadInfo threadInfo[numThreads];

	//calculate how many chars each thread needs to check
	int charPerThread=fileContent.length()/numThreads;

	timer3.startTimer();
	//create all the threads
	for(size_t i=0;i<numThreads;i++){
		//first set all the required thread info
		threadInfo[i].startIndex = i*charPerThread;
		threadInfo[i].endIndex = i*charPerThread+charPerThread;
		threadInfo[i].searchFor = searchFor;
		threadInfo[i].content = &fileContent;
		threadInfo[i].niceVal=niceValue;
		if(i==numThreads-1){//if its the last thread make sure they go to the end of the string
			threadInfo[i].endIndex = fileContent.length();
		}
		//create the thread
		pthread_create(&threads[i],nullptr,coutThread,(void *) &threadInfo[i]);
	}

	int totalCount =0 ;
	//wait for each thread to finish
	for(size_t i=0;i<numThreads;i++){
		pthread_join(threads[i],nullptr);
		//add the amount each thread found into the grand total
		totalCount+=threadInfo[i].numberFound;
	}
	//stop the timers
	timer3.endTimer();
	//display the data collected
	cout << "Found the char "<< searchFor << " " << totalCount << " times"<< endl;
	cout << "Counting time:"<< endl;
	timer3.printTimer();

	Timer forkTime;
	forkTime.startTimer();

	cout << "processing file using Forks: "<< endl;
	processWithForks(fileContent,searchFor,(int)numThreads,niceValue,charPerThread);
	forkTime.endTimer();
	forkTime.printTimer();
}

void * coutThread(void * args){
	ThreadInfo* info = (ThreadInfo*)args;//create a local var for easy reference
	nice(info->niceVal);
	string content = *info->content;//Dereference the string
	for(int i=info->startIndex;i<info->endIndex;i++){//go through each char that is assigned to this thread
		if(content[i]==info->searchFor){//if that char is what we are searching for
			info->numberFound++;//increase this threads total
		}
		volatile double x=info->numberFound;
		for(int j=0;j<10;j++){//waist time so the effects can be felt
			x/=j;
		}
	}
	return nullptr;
}

void processWithForks(string &s,char c,int numForks,int nicenes,int cpf){
#ifdef _WIN32
	//if on windows print an error becasue forking is not a thing on windows
	cerr << "FORKING NOT SUPPORTED ON WINDOWS"<<endl;
#else
	//do the same process as the threads but this time in forking
	for(int f=0;f<numForks;f++){
		int startIndex = f*cpf;
		int endIndex = f*cpf+cpf;
		if(f==numForks-1){//if its the last thread make sure they go to the end of the string
			endIndex = s.length();
		}
		if(fork()==0){
			nice(nicenes);
			volatile int numberFound =0;
			for(int i=startIndex;i<endIndex;i++){//go through each char that is assigned to this thread
				if(s[i]==c){//if that char is what we are searching for
					numberFound++;//increase this threads total
				}
				volatile double x=numberFound;
				for(int j=0;j<10;j++){//waist time so the effects can be felt
					x/=j;
				}
			}
			exit(0);
		}
	}


	while(wait(NULL)!=-1){}//wait until all the children are dead


#endif
}

//TIMER IMPLMENTATION

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
