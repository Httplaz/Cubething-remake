#pragma once
#include <thread>
#include <functional>
#include <mutex>
#include "DequeSet.h"
#include <iostream>

template <unsigned threadCount, typename Args>
class Threadpool
{
public:
	Threadpool(std::function<void(Args)> func);
	void addTask(Args task);
	void quit();
	void update();
private:
	DequeSet<Args> tasks;
	std::mutex poolMutex;
	std::thread threads[threadCount];
	std::function<void(Args)> taskFunction;
	bool working[threadCount];
	bool workFinished = false;
	bool workLocked = false;
};

template<unsigned threadCount, typename Args>
inline Threadpool<threadCount, Args>::Threadpool(std::function<void(Args)> func) : taskFunction(func)
{
	for (int i = 0; i < threadCount; i++)
		threads[i] = std::thread([&]() 
			{
				while (!workFinished)
				{
					poolMutex.lock();
					if (!tasks.empty())
					{
						Args task = tasks.popFront();
						poolMutex.unlock();
						//std::cout << "NIGGERS\n";
						taskFunction(task);
						//poolMutex.unlock();
					}
					else
						poolMutex.unlock();
				}
			});
}

template<unsigned threadCount, typename Args>
inline void Threadpool<threadCount, Args>::addTask(Args task)
{
	tasks.pushBack(task);
}

template<unsigned threadCount, typename Args>
inline void Threadpool<threadCount, Args>::quit()
{
	workFinished = true;
	for (int i = 0; i < threadCount; i++)
		threads[i].join();
}

template<unsigned threadCount, typename Args>
inline void Threadpool<threadCount, Args>::update()
{
	//if (!workLocked)
	{
		//workLocked = true;
		//poolMutex.lock();
	}
	//if (!tasks.empty())
		//poolMutex.unlock(), workLocked = false;

}


