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
	//한번에 한스레드 만
	if (_distributing.exchange(true) == true)
		return;

	//예약을 걸어준 실행할 잡들 뽑아 오기
	Vector<TimerItem> items;
	{
		//스레드 낭비방지를 위해 items에 빠르게 복사
		WRITE_LOCK;
		while (_items.empty() == false)
		{
			const TimerItem& timerItem = _items.top();
			if (now < timerItem.executeTick) // 아직 실행할때가 안됨
				break;

			items.push_back(timerItem);
			_items.pop();
		}
	}

	for (TimerItem& item : items)
	{
		//실행해야될 Job JobQueue에 넣어주기
		if (JobQueueRef owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);

		ObjectPool<JobData>::Push(item.jobData);//다쓴거 반환
	}

	//끝났으니 false
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
