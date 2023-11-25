#pragma once
#include <functional>
//람다식안에서 사이클이 일어나지 않도록
//안정장치 추가
/*-----------
	Job
-----------*/
using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : _callback(std::move(callback))
	{

	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::*memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	void Execute()
	{
		_callback();
	}
private:
	CallbackType _callback;
};

