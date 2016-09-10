#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <random>
#include <chrono>
#include <atomic>
#include <Windows.h>

std::atomic<bool> ex = true;

std::mutex vector_usage_mutex;
std::vector<int> randoms;

void numberGen()
{
	int myRandom;
	while (ex)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		myRandom = rand() % 10 + 1;
		vector_usage_mutex.lock();
		randoms.push_back(myRandom);
		vector_usage_mutex.unlock();
	}
}

void randomPrinter()
{
	while (ex)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if (randoms.size() > 1)
		{
			vector_usage_mutex.lock();
			std::cout << randoms.back() << "\n";			
			vector_usage_mutex.unlock();
		}
		
	}
}

int main(int argc, char **argv)
{
	std::thread* plaa = new std::thread(numberGen);
	std::thread* plaa1 = new std::thread(randomPrinter);

	while (ex)
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			ex = false;
		}
	}

	std::cout << "----- all numbers generated -----" << std::endl;
	for (int i = 0; i < randoms.size(); i++)
	{		
		std::cout << randoms[i] << " - ";
	}

	plaa->join();
	plaa1->join();

	bool exit = false;
	while (!exit)
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			exit = true;
		}
	}

	return 0;
}