#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

// For threads
#include <list>
#include <mutex>
#include <thread>
#include <fstream>
#include <vector>
#include <atomic>
#include <string>

#include <ctime>

struct outmessage
{
	std::string message;
	std::string sender;
	std::string timestamp;
	struct sockaddr_in si_other;
	int slen;
	int mySleep;
};

std::mutex list_usage_mutex;
std::list<outmessage> MyList;
std::atomic<bool> exiting = false;
int sleepCountR = 0;
bool startcounting = false;
std::mutex sleepCount_mutex;

void messageSender(SOCKET s)
{
	char sbuf[BUFLEN];
	outmessage sendtemp;
	while (!exiting)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (!MyList.empty())
		{
			list_usage_mutex.lock();
			sendtemp = MyList.back();
			MyList.pop_back();
			list_usage_mutex.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(sendtemp.mySleep));

			time_t t = time(0);
			struct tm * now = localtime(&t);
			std::string sendTime = "| " + std::to_string(now->tm_hour) + ":" + std::to_string(now->tm_min) + ":" + std::to_string(now->tm_sec) + " |";

			sendtemp.message += " " + sendtemp.sender + " " + sendtemp.timestamp + " " + sendTime;

			int index = 0;
			for (int i = 0; i < sendtemp.message.length(); i++)
			{
				sbuf[index++] = sendtemp.message[i];
			}
			sbuf[index++] = 0;

			if (sendto(s, sbuf, index, 0, (struct sockaddr*) &sendtemp.si_other, sendtemp.slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
		}
	}

}

void countDownFrom10()
{
	sleepCountR = 10000;
	while (!exiting)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (sleepCountR <= 100)
		{
			sleepCountR = 10000;
			startcounting = false;
		}
		else if (startcounting)
		{
			sleepCount_mutex.lock();
			sleepCountR -= 100;
			sleepCount_mutex.unlock();
		}		
	}
}

int main()
{
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	SOCKET s;

	slen = sizeof(si_other);

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::thread* myMessagerThread = new std::thread(messageSender, s);
	std::thread* countDown10Thread = new std::thread(countDownFrom10);

	int lastSleepCount = 0;
	int tempTimeCalc = 0;
	outmessage temp;

	while (1)
	{
		Sleep(100);
		printf("Waiting for data..."); fflush(stdout);

		memset(buf, '\0', BUFLEN);
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		
		// Get current time
		time_t t = time(0);
		struct tm * now = localtime(&t);

		// Calculate send delay between messages		
		if (sleepCountR == 10000 && !startcounting)
		{
			temp.mySleep = 10000;
			tempTimeCalc = 0;
			lastSleepCount = 0;
			startcounting = true;
		}
		else
		{
			sleepCount_mutex.lock();
			tempTimeCalc = 10000 - sleepCountR - lastSleepCount;
			sleepCount_mutex.unlock();
			if (tempTimeCalc < 100)
			{
				tempTimeCalc = 0;
			}

			// For next delay calculation...
			lastSleepCount += tempTimeCalc;

			// Save delay to temp
			temp.mySleep = tempTimeCalc;
		}

		// Print message
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buf);
		printf("Send-delay %d\n", tempTimeCalc);

		// Save message to temp
		if (recv_len == 0)
		{
			recv_len++;
			buf[0] = ' ';
		}
		temp.message.resize(recv_len);
		memcpy(&temp.message[0], &buf[0], recv_len);
		temp.message.pop_back();
		temp.si_other = si_other;
		temp.slen = slen;
		
		// Save timestamp and address to temp
		temp.timestamp = "| " + std::to_string(now->tm_hour) + ":" + std::to_string(now->tm_min) + ":" + std::to_string(now->tm_sec) + " |";
		temp.sender = inet_ntoa(si_other.sin_addr);

		// Save temp to message line
		list_usage_mutex.lock();
		MyList.push_front(temp);
		list_usage_mutex.unlock();
	}

	myMessagerThread->join();

	closesocket(s);
	WSACleanup();

	return 0;
}
