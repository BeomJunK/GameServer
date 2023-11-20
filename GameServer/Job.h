#pragma once
class IJob
{
public:
	virtual void Excute() {}
};

class HealJob : public IJob
{
public:
	virtual void Excute() override
	{
		cout << _target << " ���� �� " << _healValue << endl;
	}

public:
	uint64 _target = 0;
	uint32 _healValue = 0;
};


using JobRef = shared_ptr<IJob>;

class JobQueue
{
public:
	void Push(JobRef job)
	{
		WRITE_LOCK;
		_jobs.push(job);
	}
	JobRef Pop()
	{
		WRITE_LOCK;
		if(_jobs.empty())
			return nullptr;

		JobRef ret = _jobs.front();
		_jobs.pop();
		return ret;
	}
private:
	USE_LOCK
	queue<JobRef> _jobs;
};