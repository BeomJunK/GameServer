#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"
void JobTimer::Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;
	_items.push(TimerItem{ executeTick, jobData });
}

void JobTimer::Distribute(uint64 now)
{
	//�ѹ��� �ѽ����� ��
	if (_distributing.exchange(true) == true)
		return;

	//������ �ɾ��� ������ ��� �̾� ����
	Vector<TimerItem> items;
	{
		//������ ��������� ���� items�� ������ ����
		WRITE_LOCK;
		while (_items.empty() == false)
		{
			const TimerItem& timerItem = _items.top();
			if (now < timerItem.executeTick) // ���� �����Ҷ��� �ȵ�
				break;

			items.push_back(timerItem);
			_items.pop();
		}
	}

	for (TimerItem& item : items)
	{
		//�����ؾߵ� Job JobQueue�� �־��ֱ�
		if (JobQueueRef owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);

		ObjectPool<JobData>::Push(item.jobData);//�پ��� ��ȯ
	}

	//�������� false
	_distributing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;
	while (_items.empty() == false)
	{
		const TimerItem& timerItem = _items.top();
		ObjectPool<JobData>::Push(timerItem.jobData);
		_items.pop();
	}
}
