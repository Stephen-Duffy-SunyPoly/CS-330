/*Author: Stephen Duffy duffysd
 *HW3_Basic_Forking
 *CS330 2pm TR
 * there were no more then 1 small difference each run until I turned on a program to peg my CPU at 100%. then many many differences
 */
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

void forkRun();

int main(){
	//run the experiment 12 times
	for(int i=0;i<12;i++){
		forkRun();
	}
	return EXIT_SUCCESS;
}

void forkRun(){
	//for every char in the alfabet
	for(char c = 'A'; c < 'Z'+1 ; c++){
		//create child to do manual labor
		if(fork() == 0){//if this is the child
			cout << c;//print the current char(manual labor)
			exit(0);//sacrifice your self for the greater good
		}
	}

	while(wait(NULL)!=-1){}//wait until all the children are dead
	cout << endl;//end the line
}
