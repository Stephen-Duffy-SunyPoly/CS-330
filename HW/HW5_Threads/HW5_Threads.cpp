//Author: Stephen Duffy duffysd
//HW5 threads
//CS330 2pm TR
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <fstream>
#include <sys/time.h>
using namespace std;

//a class to hold all the timerings
typedef class Timer{//timer from HW2
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
} ThreadInfo;

void * coutThread(void * args);

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

	cout << endl << "Searching for char "<<searchFor <<" using " << numThreads << " threads in the file " << inputFileName << endl;

	//timer 1: total program time
	//timer 2: read file time
	//timer 3: search time
	Timer timer1,timer2,timer3;
	timer1.startTimer();
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
	timer1.endTimer();
	timer3.endTimer();
	//display the data collected
	cout << "Found the char "<< searchFor << " " << totalCount << " times"<< endl;
	cout << "File read time:"<< endl;
	timer2.printTimer();
	cout << "Counting time:"<< endl;
	timer3.printTimer();
	cout << "Total runtime:"<< endl;
	timer1.printTimer();
}

void * coutThread(void * args){
	ThreadInfo* info = (ThreadInfo*)args;//create a local var for easy reference
	string content = *info->content;//Dereference the string
	for(int i=info->startIndex;i<info->endIndex;i++){//go through each char that is assigned to this thread
		if(content[i]==info->searchFor){//if that char is what we are searching for
			info->numberFound++;//increase this threads total
		}
	}
	return nullptr;
}


//timer implementation from HW2
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
