#include <thread>
#include <iostream>
#include <mutex>
#include <list>

std::mutex cout_mutex;

void MyThread(int id)
{
	cout_mutex.lock();
	std::cout << "Hello world from thread =" << id << "\n";
	cout_mutex.unlock();
}

int main(int argc, char **argv)
{
	std::list<std::thread *> myList;
	
	for (int i = 0; i < 3; i++)
	{
		myList.push_back(new std::thread(MyThread, i));
	}
	
	for (auto i : myList)
	{
		i->join();
	}

	return 0;
}