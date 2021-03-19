#pragma once

#include <vector>
#include <mutex>


class Task
{
public:
	virtual int Execute() = 0;

private:

};

class TestTask : public Task
{
public:
	TestTask();
	~TestTask();

	int Execute();
private:
	int _a;
	int _b;
	int _result;
};

class Thread
{
public:
	Thread(unsigned int uniqueId);
	~Thread();

	bool ExecuteTask(Task* t);
	bool WakeUp();
	bool IsFinished();

private:
	static unsigned int __stdcall threadMain(void* params);
	uintptr_t _ThreadHandle;
	void* _EventHandle;
	std::mutex _Mutex;


	unsigned int _ID;
	bool _IsRunning;
	Task* _CurrentTask;
};

class ThreadPool
{
public:
	ThreadPool(unsigned int numThreads);
	~ThreadPool();

	void AddTask(Task* task);

private:
	Thread* getAvailableThread();

	std::vector<Thread*> _Threads;
};

