#pragma once
#include <string>
#include <list>
#include <mutex>
#include <thread>
#include <fstream>
#include <vector>
#include <atomic>

class MyLogger
{
public:
	MyLogger();
	~MyLogger();

	void addMessage(std::string message);
private:
	std::mutex list_usage_mutex;
	std::list<std::string> MyList;
	std::atomic<bool> exiting = false;

	std::thread* myWriterThread;
	void charToFilePrinter();
};
