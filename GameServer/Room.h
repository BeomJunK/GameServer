#pragma once
#include "Job.h"

class Room
{
    //싱글 스레드라 가정
public:
    void Enter(PlayerRef player);
    void Leave(PlayerRef player);
    void Broadcast(SendBufferRef sendBuffer);


    //멀티스레드에서 일감접근해 사용
public:
    void FlushJob();

    template<typename T, typename Ret, typename... Args>
    void PushJob(Ret(T::* memFunc)(Args...), Args... args)
    {
        auto job = MakeShared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
        _jobs.Push(job);
    }
private:
    USE_LOCK
    map<uint64, PlayerRef> _players;
    JobQueue _jobs;
};

extern Room GRoom;
