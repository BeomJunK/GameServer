#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

/*------------
 *  Lock
------------*/


//writeCount로 관리 1이상이라면 누군가 읽고 있다!
void Lock::WriteLock(const char* name)
{
#if _DEBUG
        GDeadLockProfiler->PushLock(name);
#endif
    //동일한 스레드가 소유하고 있다면 무조건 성공
    //동일 스레드라면 꼬일 염려가 없기 때문에?
    const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
    if(LThreadId == lockThreadId)
    {
        _writeCount++;
        return;
    }

    //아무도 소유 및 공유하고 있지 않을때 경합해서 소유권 획득
    const int64 beginTick = ::GetTickCount64();
    //앞에서 8비트 값만 쓰겠다!
    const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
    while (true)
    {
        for(int spinCount =0; spinCount < MAX_SPIN_COUNT; spinCount++)
        {
            uint32 expected = EMPTY_FLAG;
            //아무도 쓰지 않고 있다!
            //writeCount++
            if(_lockFlag.compare_exchange_strong(OUT expected, desired))
            {
                //결론 : 애초에 EMPTY상태가 아니라면 뺑뺑이 돌기때문에 밑에 연산은 상관x
                //만약 여기로 안들어와서 실패 한다면
                //_lockFlag에 들어가서 다시비교를 하게 되나?
                //desired = _lockFlag;
                _writeCount++;
                return;
            }
        }
        
        if(::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
            CRASH("LOCK TIMEOUT");

        this_thread::yield();
    }
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
    GDeadLockProfiler->PopLock(name);
#endif
    //read락을 다풀기전엔 언락 불가
    if((_lockFlag.load() & READ_COUNT_MASK ) != 0)
        CRASH("INVALID_UNLOCK_ORDER");

    const int32 lockCount = --_writeCount;
    if(lockCount == 0)
        _lockFlag.store(EMPTY_FLAG);
     
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
    GDeadLockProfiler->PushLock(name);
#endif
    //동일한 스레드가 소유하고 있다면 무조건 성공
    //동일 스레드라면 꼬일 염려가 없기 때문에?
    const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK ) >> 16;
    if(LThreadId == lockThreadId)
    {
        _lockFlag.fetch_add(1);
        return;
    }

    const int64 beginTick = GetTickCount64();
    
    //아무도 소유하고있지않을떄 경합해서 공유카운트를 올린다
    while(true)
    {
        for(int spinCount = 0; spinCount<MAX_SPIN_COUNT; spinCount++)
        {
            //아무도 write를 하고있지 않음
            uint32 expected = (_lockFlag.load() & READ_COUNT_MASK );

            //desired : expected에서 +1해서 플래그 리드상태로 바까줌
            if(_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
                return;
        }

        if(GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
            CRASH("LOCK_TIMEOUT");

        this_thread::yield();
    }
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
    GDeadLockProfiler->PopLock(name);
#endif
    if((_lockFlag.fetch_sub(1) & READ_COUNT_MASK)== 0)
        CRASH("MULTIPLE_UNLOCK");
}
