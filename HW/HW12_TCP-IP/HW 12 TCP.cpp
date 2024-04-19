//Author: Stephen Duffy duffysd
//HW12 TPC
//CS330 2pm TR

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdbool>

#define BUFFER_SIZE 200
using namespace std;

void client();
void server();

int main() {
	char iset;
	//ask wether this is the server or client
	while(iset!='S'&&iset!='C'){
		cout << "(S)erver or (C)lient?" << endl;

		cin >> iset;
	}

	if(iset == 'C'){
		client();
	}else{
		server();
	}
	return 0;
}

void client(){
	cout << "IP?: ";
	string ip;
	cin >> ip;
	cout << "port?: ";
	int port;
	cin >> port;
	cout << endl;

	int net_socket;
	struct sockaddr_in servaddr;

	// socket create and verification
	net_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(net_socket==-1){
		cerr << "SOCKET ERROR"<<endl;
		return;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	servaddr.sin_port = htons(port);

	if (connect(net_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		cerr << "Connection Failed!"<<endl;
		return;
	}

	//communcate here
	bool exited = false;

	char inputBuffer[BUFFER_SIZE+1];
	string output;

	getline(cin,output);//clear reaming std in
	while(!exited){
		cout << "Send message to server: ";
		getline(cin,output);

		write(net_socket,output.c_str(),output.length());

		cout << "Waiting for server to respond"<<endl;

		ssize_t inLen = read(net_socket,inputBuffer,BUFFER_SIZE);
		inputBuffer[inLen] = '\0';// put a null byte at the end of the input in case one was not provided

		string input(inputBuffer); // convert into a c++ string
		if(input == "exit"){
			exited = true;
		}
		cout <<endl<< "Server Message: " << input << endl;
	}


	close(net_socket);
}
void server(){
	cout << "port?: ";
	int port;
	cin >> port;
	cout << endl;

	int net_socket,client;
	struct sockaddr_in servaddr,clientaddr;
	// socket create and verification
	net_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(net_socket==-1){
		cerr << "SOCKET ERROR"<<endl;
		return;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if ((bind(net_socket, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		cerr <<"BINDING ERROR"<<endl;
		return;
	}

	if ((listen(net_socket, 5)) != 0) {
		cerr <<"LISTEN ERROR"<<endl;
		return;
	}

	socklen_t len = sizeof(clientaddr);

	cout << "Waiting for client to connect"<<endl;
	client = accept(net_socket, (struct sockaddr*)&clientaddr, &len);
	if (client < 0) {
		cerr << "ACCEPT FAILED"<<endl;
		return;
	}
	//communicate here
	cout << "client connected"<<endl;

	bool exited = false;

	char inputBuffer[BUFFER_SIZE+1];
	string output;
	getline(cin,output);//clear reaming std in

	while(!exited){
		cout << "waiting for client to respond" << endl;

		ssize_t inLen = read(client,inputBuffer,BUFFER_SIZE);
		inputBuffer[inLen] = '\0';

		string input(inputBuffer);
		cout <<endl<< "Message from client: "<< input << endl;

		cout << "Send message to client: ";
		getline(cin,output);

		write(client,output.c_str(),output.length());

		if(output == "exit"){
			exited=true;
		}

	}

	close(net_socket);
}
