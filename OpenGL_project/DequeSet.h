#pragma once
#include <deque>
#include <set>
#include <mutex>
template <typename T>
class DequeSet
{
public:
	void pushBack(T obj);
	T popFront();
	bool empty();
private:
	std::mutex mutexT;
	std::deque<T> dequeT;
	std::set<T> setT;
};

template<typename T>
inline void DequeSet<T>::pushBack(T obj)
{
	mutexT.lock();
	if (!setT.count(obj))
	{
		setT.insert(obj);
		dequeT.push_back(obj);
	}
	mutexT.unlock();
}

template<typename T>
inline T DequeSet<T>::popFront()
{
	mutexT.lock();
	T obj = dequeT.front();
	dequeT.pop_front();
	setT.erase(obj);
	mutexT.unlock();
	return obj;
}

template<typename T>
inline bool DequeSet<T>::empty()
{
	mutexT.lock();
	bool p = dequeT .empty();
	mutexT.unlock();
	return p;
}
