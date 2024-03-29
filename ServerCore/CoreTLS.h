#pragma once
#include <stack>
/*------------------
	CoreTLS
------------------*/

extern thread_local uint32 LThreadId;
extern thread_local uint64 LEndThickCount;
extern thread_local std::stack<int32> LLockStack;
extern thread_local SendBufferChunkRef LSendBufferChunk; 
extern thread_local class JobQueue* LCurrentJobQueue;
