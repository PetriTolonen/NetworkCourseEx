#include <iostream>
#include <string>
#include "MyLogger.h"

int main(int argc, char **argv)
{
	MyLogger* log = new MyLogger();

	bool exit = false;
	while (!exit)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "exit")
		{
			exit = true;
		}
		else
		{
			log->addMessage(input);
		}
	}
	log->~MyLogger();
	return 0;
}