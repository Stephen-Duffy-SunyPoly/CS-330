//Author: Stephen Duffy duffysd
//HW5 Code_Timer_Testcases
//CS330 2pm TR
#include <iostream>
#include <sys/time.h>

using namespace std;

//a class to hold all the timerings
typedef class Timer{
	public:
		//prototype for required functions
		void startTimer();
		void endTimer();
		void printTimer();

		void setStart(struct timeval st);
		void setEnd(struct timeval et);
	private:
		struct timeval startime;
		struct timeval endtime;
} Timer;

void testCase();

int main(){
	testCase();
	Timer t;
	//start the timer
	t.startTimer();
	cout << "hello world"<< endl;
	//waste some time
	for(int i=0;i<1000000;i++){
		cout << "WASTING TIME! " << i << "\r";
		cout.flush();
		//overriete the same line so we don't spam the screen
	}
	cout << endl;
	//end the timer
	t.endTimer();
	//print the time elapsed
	t.printTimer();
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

void Timer::setStart(struct timeval st){
	startime=st;
}
void Timer::setEnd(struct timeval et){
	endtime=et;
}

void testCase(){
	struct timeval start;
	struct timeval end;
	//set the set values for the test case
	start.tv_sec = 3;
	start.tv_usec = 999999;
	end.tv_sec = 4;
	end.tv_usec = 1;
	Timer tc;
	tc.setStart(start);
	tc.setEnd(end);
	cout << "Test Case start: " << start.tv_sec  << "s " << start.tv_usec << "us " << endl;
	cout << "Test Case end: " << end.tv_sec << "s " << end.tv_usec << "us " << endl;
	cout << "Test Case: ";//print the results
	tc.printTimer();
}
