#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <chrono>
#include <atomic>
#include <fstream>
#include <string>

std::atomic<bool> ex = true;

std::mutex vector_usage_mutex;
std::vector<char> inputString;

void stringSaver()
{
	while (ex)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "exit")
		{
			ex = false;
		}
		else
		{
			vector_usage_mutex.lock();
			
			input = input + "\n";
			for (int i = 0; i < input.size(); i++)
			{
				inputString.insert(inputString.begin(), input[i]);
			}
			vector_usage_mutex.unlock();
		}
	}
}

void charToFilePrinter()
{
	std::fstream fs;
	fs.open("stringlogger.txt", std::fstream::out);
	fs.close();

	std::vector<char> toFile;
	bool hasWrittenExitMessage = false;
	while (!hasWrittenExitMessage)
	{
		if (!ex)
		{
			vector_usage_mutex.lock();
			std::string exitString = "\n ||-- Progman exiting, writing all remaining text --|| \n";
			for (int i = exitString.size() - 1; i >= 0; i--)
			{
				inputString.push_back(exitString[i]);
			}
			hasWrittenExitMessage = true;
			vector_usage_mutex.unlock();
		}

		int howMany = inputString.size();
		if (howMany > 0)
		{
			if (howMany < 10 || hasWrittenExitMessage)
			{
				vector_usage_mutex.lock();
				for (int i = 0; i < howMany; i++)
				{
					toFile.push_back(inputString.back());
					inputString.pop_back();
				}
				vector_usage_mutex.unlock();

				fs.open("stringlogger.txt", std::fstream::out | std::fstream::app);
				for (int i = 0; i < toFile.size(); i++)
				{
					fs << toFile[i];
				}
				toFile.clear();
				fs.close();
			}
			else
			{
				vector_usage_mutex.lock();
				for (int i = 0; i < 10; i++)
				{
					toFile.push_back(inputString.back());
					inputString.pop_back();
				}
				vector_usage_mutex.unlock();

				fs.open("stringlogger.txt", std::fstream::out | std::fstream::app);
				for (int i = 0; i < toFile.size(); i++)
				{
					fs << toFile[i];
				}
				toFile.clear();
				fs.close();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int main(int argc, char **argv)
{
	std::thread* plaa = new std::thread(stringSaver);
	std::thread* plaa1 = new std::thread(charToFilePrinter);

	plaa->join();
	plaa1->join();

	return 0;
}