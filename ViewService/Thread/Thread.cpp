/*
 * Thread.cpp
 *
 *  Created on: 2017-3-27
 *      Author: Administrator
 */

#include "Thread.h"

Thread::Thread()
{
	this->isInterript = false;
}

Thread::~Thread()
{
	if (!this->isInterrupted())
	{
		this->interrupt();
	}

	if (this->th.joinable())
	{
		this->th.join();
	}
}

void Thread::start()
{
	std::thread thr(std::bind(&Thread::run, this));
	this->th = std::move(thr);
}

std::thread::id Thread::getId()
{
	return this->th.get_id();
}

void Thread::interrupt()
{
	this->isInterript = true;
}

bool Thread::isInterrupted()
{
	return this->isInterript;
}

void Thread::join()
{
	this->th.join();
}

void Thread::run()
{
}
