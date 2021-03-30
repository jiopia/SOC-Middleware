/*
 * Thread.h
 *
 *  Created on: 2017-3-27
 *      Author: Administrator
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <iostream>
#include <thread>
#include <atomic>
#include <functional>

class Thread
{
public:
	Thread();
	virtual ~Thread();

	void Start();
	std::thread::id GetId();
	void Interrupt();
	bool IsInterrupted();
	void Join();
	virtual void Run();

private:
	std::atomic<bool> isInterript;
	std::thread th;
};

#endif /* THREAD_H_ */