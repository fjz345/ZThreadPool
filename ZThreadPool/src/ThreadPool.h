#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

class Task
{
public:
	virtual int Execute() = 0;

protected:
	std::mutex _Mutex;

private:
	friend class ThreadPool;
	friend class Thread;
};

class TestTask : public Task
{
public:
	TestTask();
	~TestTask();

	int Execute();

	int _a;
	int _b;
	int _result;
private:
	
};

class Thread
{
public:
	Thread(unsigned int uniqueId);
	~Thread();

	bool IsFinished();

private:
	friend class ThreadPool;

	static unsigned int __stdcall threadMain(void* params);
	uintptr_t _ThreadHandle;
	void* _EventHandle;

	unsigned int _ID;
	bool _IsRunning;

	std::mutex _Mutex;
	Task* _CurrentTask;
};

static std::queue<Task*> s_TaskQueue;
static std::mutex s_TaskQueueMutex;
static std::condition_variable s_Condition;
static unsigned int s_ActiveThreads;
static std::mutex s_ActiveThreadsMutex;
static std::condition_variable s_ActiveThreadsCondition;
class ThreadPool
{
public:
	ThreadPool(unsigned int numThreads);
	~ThreadPool();

	void WaitForAllThreads();

	void AddTask(Task* task);

	

private:
	std::vector<Thread*> _Threads;
	
	
};

