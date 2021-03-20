#include "ThreadPool.h"

#include <windows.h>
#include <process.h>
#include <synchapi.h>

#include <iostream>

ThreadPool::ThreadPool(unsigned int numThreads)
{
	for (unsigned int i = 0; i < numThreads; i++)
	{
		_Threads.push_back(new Thread(i));
	}

	s_ActiveThreads = 0;
}

ThreadPool::~ThreadPool()
{
	for (auto p : _Threads)
	{
		delete p;
	}
}

void ThreadPool::WaitForAllThreads()
{
	while (true)
	{
		volatile size_t queueSize = s_TaskQueue.size();
		if (queueSize == 0)
			break;
	}

	while (s_TaskQueue.size() != 0);

	std::cout << "s_TaskQueue.size() = " << s_TaskQueue.size() << std::endl;

	{
		std::unique_lock<std::mutex> lock(s_ActiveThreadsMutex);
		s_ActiveThreadsCondition.wait(lock, [] {return s_ActiveThreads == 0; });
	}

	return;
}

void ThreadPool::AddTask(Task* task)
{
	{
		s_TaskQueueMutex.lock();
		s_TaskQueue.push(task);
		s_TaskQueueMutex.unlock();
	}
	s_Condition.notify_one();
}

Thread::Thread(unsigned int uniqueId)
	: _ID(uniqueId)
{
	_IsRunning = true;
	_CurrentTask = nullptr;

	// Create an event for the thread
	_EventHandle = CreateEvent(
		NULL,   // default security attributes
		FALSE,  // manual-reset event
		FALSE,  // initial state is nonsignaled
		NULL	// object m_Name
	);

	_ThreadHandle = _beginthreadex(0, 0, threadMain, static_cast<void*>(this), 0, 0);

	std::cout << "Created Thread: " << _ThreadHandle << std::endl;
}

Thread::~Thread()
{
	std::cout << "Killing Thread: " << _ThreadHandle << std::endl;

	bool ret = CloseHandle((HANDLE)_ThreadHandle);
	if (ret == 0)
	{
		DebugBreak();
	}
}

unsigned int __stdcall Thread::threadMain(void* params)
{
	Thread* threadInstance = static_cast<Thread*>(params);

	while (true)
	{
		// Get a job
		{
			std::unique_lock<std::mutex> lock(s_TaskQueueMutex);
			s_Condition.wait(lock, [] {return !s_TaskQueue.empty(); });
			threadInstance->_CurrentTask = s_TaskQueue.front();
			s_TaskQueue.pop();
		}

		{
			s_ActiveThreadsMutex.lock();
			s_ActiveThreads++;
			s_ActiveThreadsMutex.unlock();
		}

		// Work
		threadInstance->_CurrentTask->Execute();

		// Finished the task
		threadInstance->_CurrentTask = nullptr;
		{
			s_ActiveThreadsMutex.lock();
			s_ActiveThreads--;
			s_ActiveThreadsMutex.unlock();
		}
		s_ActiveThreadsCondition.notify_all();
	}

	return 0;
}

bool Thread::IsFinished()
{
	bool isFinished = _CurrentTask == nullptr;
	return isFinished;
}

TestTask::TestTask()
	: _a(2), _b(6), _result(0)
{
}

TestTask::~TestTask()
{
}

int TestTask::Execute()
{
	return _result = _a + _b;
}