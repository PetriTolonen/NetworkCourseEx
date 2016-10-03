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

struct client
{
	struct sockaddr_in si_other;
	std::thread* myConnectThread;
	int playerNumber;
};

struct outmessage
{
	std::string message;
	std::string sender;
	std::string timestamp;
	int slen;
};

struct tableSlot
{
	char coord[2];
	int player;
};

namespace
{
	std::mutex list_usage_mutex;
	std::list<outmessage> MyMessageList;
	std::vector<client> MyClientList;
	std::atomic<bool> exiting = false;
	SOCKET serversocket;

	std::vector<tableSlot> table;
}

void messageSender()
{
	char sbuf[BUFLEN];
	outmessage sendtemp;
	while (!exiting)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (!MyMessageList.empty())
		{
			list_usage_mutex.lock();
			sendtemp = MyMessageList.back();
			MyMessageList.pop_back();
			list_usage_mutex.unlock();

			int index = 0;
			for (int i = 0; i < sendtemp.message.length(); i++)
			{
				sbuf[index++] = sendtemp.message[i];
			}
			
			sbuf[index++] = '\n';

			int three = 0;
			// Add full table to message
			for (auto tableSlot : table)
			{
				if (tableSlot.player == 0)
				{
					sbuf[index++] = '#';
				}
				else if (tableSlot.player == 1)
				{
					sbuf[index++] = 'X';
				}
				else if (tableSlot.player == 2)
				{
					sbuf[index++] = 'O';
				}
				three++;
				if (three == 3)
				{
					sbuf[index++] = '\n';
					three = 0;
				}				
			}
			
			sbuf[index++] = 0;
			
			for (int i = 0; i < MyClientList.size(); i++)
			{
				if (sendto(serversocket, sbuf, index, 0, (struct sockaddr*) &MyClientList[i].si_other, sendtemp.slen) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code : %d", WSAGetLastError());
					exit(EXIT_FAILURE);
				}
			}
		}
	}
}


int main()
{
	struct sockaddr_in server, si_other;
	WSADATA wsa;

	int slen;

	char buf[BUFLEN];
	int recv_len;

	slen = sizeof(si_other);

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if ((serversocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(serversocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	// Start sending messages to all
	std::thread* myMessagerThread = new std::thread(messageSender);

	bool found = false;
	int currentPlayer = 1;
	int newPlayerNumber = 0;
	bool correctPlayer = false;

	tableSlot sl1;
	sl1.coord[0] = 'A';
	sl1.coord[1] = '1';
	sl1.player = 0;
	table.push_back(sl1);
	tableSlot sl2;
	sl2.coord[0] = 'A';
	sl2.coord[1] = '2';
	sl2.player = 0;
	table.push_back(sl2);
	tableSlot sl3;
	sl3.coord[0] = 'A';
	sl3.coord[1] = '3';
	sl3.player = 0;
	table.push_back(sl3);
	tableSlot sl4;
	sl4.coord[0] = 'B';
	sl4.coord[1] = '1';
	sl4.player = 0;
	table.push_back(sl4);
	tableSlot sl5;
	sl5.coord[0] = 'B';
	sl5.coord[1] = '2';
	sl5.player = 0;
	table.push_back(sl5);
	tableSlot sl6;
	sl6.coord[0] = 'B';
	sl6.coord[1] = '3';
	sl6.player = 0;
	table.push_back(sl6);
	tableSlot sl7;
	sl7.coord[0] = 'C';
	sl7.coord[1] = '1';
	sl7.player = 0;
	table.push_back(sl7);
	tableSlot sl8;
	sl8.coord[0] = 'C';
	sl8.coord[1] = '2';
	sl8.player = 0;
	table.push_back(sl8);
	tableSlot sl9;
	sl9.coord[0] = 'C';
	sl9.coord[1] = '3';
	sl9.player = 0;
	table.push_back(sl9);

	outmessage temp;
	while (!exiting)
	{
		Sleep(100);
		printf("Waiting for data..."); fflush(stdout);

		memset(buf, '\0', BUFLEN);
		if ((recv_len = recvfrom(serversocket, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		int index = 0;
		for (auto ListClient : MyClientList)
		{
			index++;
			if (inet_ntoa(ListClient.si_other.sin_addr) == inet_ntoa(si_other.sin_addr))
			{
				if (ntohs(ListClient.si_other.sin_port) == ntohs(si_other.sin_port))
				{
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			client tempc;
			tempc.si_other = si_other;
			tempc.playerNumber = newPlayerNumber++;
			MyClientList.push_back(tempc);
		}

		// Print message
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buf);

		if (found)
		{
			if ((index) == currentPlayer)
			{
				correctPlayer = true;
			}
			else
			{
				correctPlayer = false;
			}
		}

		bool correctMove = false;
		if (correctPlayer)
		{
			for (auto &tabbleSlot : table)
			{
				if (buf[0] == tabbleSlot.coord[0] && buf[1] == tabbleSlot.coord[1])
				{
					if (tabbleSlot.player == 0)
					{
						tabbleSlot.player = currentPlayer;
						correctMove = true;
					}
				}
			}

			if (correctMove)
			{
				currentPlayer = ((currentPlayer) % 2) + 1;
			}			
		}

		if (correctMove)
		{
			// Save message to temp
			if (recv_len == 0)
			{
				recv_len++;
				buf[0] = ' ';
			}
			temp.message.resize(recv_len);
			memcpy(&temp.message[0], &buf[0], recv_len);
			temp.message.pop_back();
			temp.slen = slen;

			// Save temp to message line
			list_usage_mutex.lock();
			MyMessageList.push_front(temp);
			list_usage_mutex.unlock();
		}
		else
		{
			temp.message = "Not a correct move, try again with A - C and 1 - 3. Example B3. Current Player = " + std::to_string(currentPlayer);
			temp.slen = 100; // TODO: calculate correct lenght...

			// Save temp to message line
			list_usage_mutex.lock();
			MyMessageList.push_front(temp);
			list_usage_mutex.unlock();
		}
	}

	myMessagerThread->join();

	closesocket(serversocket);

	WSACleanup();

	return 0;
}
