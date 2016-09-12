#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#ifdef MINGWTHREAD
#  include "mingw.thread.h"
#  include "mingw.mutex.h"
#else
#  include <thread>
#  include <mutex>
#endif
#include <list>
#include <string>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

std::list<std::string> msgbuf;
std::mutex msgbuf_mtx;

struct sockaddr_in si_other, si_this;
int s, slen = sizeof(si_other);


void ReceiveThread(int s) {
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buf[BUFLEN];

	while (1) {
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR) {
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		msgbuf_mtx.lock();
		msgbuf.push_back(buf);
		msgbuf_mtx.unlock();
	}
}

void SendThread()
{
	char buf[BUFLEN];
	char message[BUFLEN];

	while (1)
	{
		printf("Enter message : ");
		gets(message);

		if (sendto(s, message, strlen(message) + 1, 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR) {
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}	
}

int main(void) {
	WSADATA wsa;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

	//setup address structure
	memset((char *)&si_this, 0, sizeof(si_this));
	si_this.sin_family = AF_INET;
	si_this.sin_port = htons(PORT + 1);
	si_this.sin_addr.S_un.S_addr = INADDR_ANY;

	//Bind socket
	if (bind(s, (struct sockaddr *)&si_this, sizeof(si_this)) == SOCKET_ERROR) {
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::thread recthr(ReceiveThread, s);
	std::thread sendthr(SendThread);
	//start communication
	while (1) {
		

		Sleep(100);
		msgbuf_mtx.lock();
		while (!msgbuf.empty()) {
			std::string s = msgbuf.front();
			msgbuf.pop_front();
			printf("%s\n", s.c_str());
		}
		msgbuf_mtx.unlock();
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
