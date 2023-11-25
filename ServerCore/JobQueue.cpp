#include "pch.h"
#include "JobQueue.h"

/*------------
	JobQueue
--------------*/
void JobQueue::Push(JobRef job, bool pushOnly)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK

	//���� ó������ ���� �������̴�.
	//���� �⵵ ��� ó���ϰ� �Ѵ�.

	if(prevCount == 0)
	{
		//�̹̽������� jobQueue�������� ����
		if(LCurrentJobQueue == nullptr && pushOnly == false)
			Execute();
		else
		{
			//�����ִ� �ٸ� jobqueue �ѱ�
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		//PopAll���� ���� ���� �̵��� �ϴ°� ���?
		//�����ʹѱ� �������
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
		//�ϰ��� ��~�� ������ �׳� �ѱ�� ���� ������
		//�� �ɸ��Ÿ� �����ϰ�
		const uint64 now = ::GetTickCount64();
		if (now >= LEndThickCount)
		{
			LCurrentJobQueue = nullptr;

			GGlobalQueue->Push(shared_from_this());
			break;
		}
	} 
}

