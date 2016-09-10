#include "MyLogger.h"

MyLogger::MyLogger()
{
	// Owerwriting file for convenience on this test program...
	std::fstream fs;
	fs.open("messagelog.txt", std::fstream::out);
	fs.close();

	// Starting file writing thread
	myWriterThread = new std::thread(&MyLogger::charToFilePrinter, this);
}

MyLogger::~MyLogger()
{
	exiting = true;
	myWriterThread->join();
}

void MyLogger::addMessage(std::string message)
{
	list_usage_mutex.lock();

	MyList.push_front(message);

	list_usage_mutex.unlock();
}

void MyLogger::charToFilePrinter()
{
	std::fstream fs;
	std::string temp;
	std::vector<char> toFile;
	fs.open("messagelog.txt", std::fstream::out | std::fstream::app);
	while (!MyLogger::exiting)
	{
		if (!MyList.empty())
		{
			list_usage_mutex.lock();
			temp = MyList.back();
			int howManyChar = MyList.back().size();
			MyList.pop_back();			
			list_usage_mutex.unlock();

			for (int i = 0; i < howManyChar; i++)
			{
				toFile.push_back(temp.back());
				temp.pop_back();
			}

			for (int i = howManyChar - 1; i >= 0; i--)
			{
				fs << toFile[i];
			}
			fs << "\n";
			toFile.clear();
		}
	}
	fs.close();
}
