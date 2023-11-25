#include "pch.h"
/*------------------
	ThreadManager
-------------------*/

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void()> callback)
{
	LockGuard guard(_lock);

	_threads.push_back( thread([=]
	{
		InitTLS();
		callback();
		DestroyTLS();
	}));
}

void ThreadManager::Join()
{
	for( thread& t : _threads)
	{
		if(t.joinable())
			t.join();
	}

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32>  SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		uint64 now = ::GetTickCount64();
		if (now > LEndThickCount)
			break;

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();


	}
}

void ThreadManager::DistributeReserve()
{
	const uint64 now = ::GetTickCount64();

	GJobTimer->Distribute(now);
}
