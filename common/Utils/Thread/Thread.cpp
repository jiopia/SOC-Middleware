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
	if (!this->IsInterrupted())
	{
		this->Interrupt();
	}

	if (this->th.joinable())
	{
		this->th.join();
	}
}

void Thread::Start()
{
	std::thread thr(std::bind(&Thread::Run, this));
	this->th = std::move(thr);
}

std::thread::id Thread::GetId()
{
	return this->th.get_id();
}

void Thread::Interrupt()
{
	this->isInterript = true;
}

bool Thread::IsInterrupted()
{
	return this->isInterript;
}

void Thread::Join()
{
	this->th.join();
}

void Thread::Run()
{
}
