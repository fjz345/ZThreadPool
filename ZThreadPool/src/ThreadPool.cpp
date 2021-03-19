#include "ThreadPool.h"

#include <windows.h>
#include <process.h>
#include <synchapi.h>

ThreadPool::ThreadPool(unsigned int numThreads)
{
	for (unsigned int i = 0; i < numThreads; i++)
	{
		_Threads.push_back(new Thread(i));
	}
}

ThreadPool::~ThreadPool()
{
	for (auto p : _Threads)
	{
		delete p;
	}
}

void ThreadPool::AddTask(Task* task)
{
	Thread* thread = getAvailableThread();
	thread->ExecuteTask(task);
}

Thread* ThreadPool::getAvailableThread()
{
	for (Thread* t : _Threads)
	{
		if (t->IsFinished())
			return t;
	}

	// If none free, choose first
	return _Threads[0];
}

Thread::Thread(unsigned int uniqueId)
	: _ID(uniqueId)
{
	_IsRunning = true;
	_CurrentTask = nullptr;

	_ThreadHandle = _beginthreadex(0, 0, threadMain, static_cast<void*>(this), 0, 0);

	// Create an event for the thread
	_EventHandle = CreateEvent(
		NULL,   // default security attributes
		FALSE,  // manual-reset event
		FALSE,  // initial state is nonsignaled
		NULL	// object m_Name
	);

	WakeUp();



}

Thread::~Thread()
{
	_IsRunning = false;
	WakeUp();
}

bool Thread::ExecuteTask(Task* t)
{
	_Mutex.lock();
	_CurrentTask = t;
	_Mutex.unlock();
	
	WakeUp();

	return true;
}

bool Thread::WakeUp()
{
	return SetEvent(_EventHandle);
}

unsigned int __stdcall Thread::threadMain(void* params)
{
	Thread* threadInstance = static_cast<Thread*>(params);

	while (threadInstance->_IsRunning)
	{
		// Sleep until time to work
		WaitForSingleObject(
			threadInstance->_EventHandle, // event handle
			INFINITE);    // indefinite wait


		

		// Work
		threadInstance->_Mutex.lock();
		threadInstance->_CurrentTask->Execute();
		threadInstance->_Mutex.unlock();


		// Finished the task
		threadInstance->_Mutex.lock();
		threadInstance->_CurrentTask = nullptr;
		threadInstance->_Mutex.unlock();
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