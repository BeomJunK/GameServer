#include "pch.h"
#include "JobQueue.h"

/*------------
	JobQueue
--------------*/
void JobQueue::Push(JobRef job, bool pushOnly)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK

	//내가 처음잡을 넣은 스레드이다.
	//남은 잡도 모두 처리하게 한다.

	if(prevCount == 0)
	{
		//이미실행중인 jobQueue가없으면 실행
		if(LCurrentJobQueue == nullptr && pushOnly == false)
			Execute();
		else
		{
			//여유있는 다른 jobqueue 넘김
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		//PopAll에서 복사 말고 이동을 하는게 어떤가?
		//포인터닌까 상관없음
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; ++i)
			jobs[i]->Execute();

		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}
		//일감이 너~무 많을때 그냥 넘기고 빠져 나오기
		//렉 걸릴거면 공평하게
		const uint64 now = ::GetTickCount64();
		if (now >= LEndThickCount)
		{
			LCurrentJobQueue = nullptr;

			GGlobalQueue->Push(shared_from_this());
			break;
		}
	} 
}

