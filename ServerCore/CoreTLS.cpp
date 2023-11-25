#include "pch.h"
#include "CoreTLS.h"

/*------------------
	CoreTLS
------------------*/
//스레드별로 가지고있는 공간
thread_local uint32 LThreadId = 0;
thread_local uint64 LEndThickCount = 0;
thread_local std::stack<int32> LLockStack;
thread_local SendBufferChunkRef LSendBufferChunk;
thread_local JobQueue* LCurrentJobQueue = nullptr;
