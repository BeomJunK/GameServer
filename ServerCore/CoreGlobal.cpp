#include "pch.h"


ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
GlobalQueue* GGlobalQueue = nullptr;

class CoreGlobal
{
public:
    CoreGlobal()
    {
        GThreadManager = new ThreadManager();
        GMemory = new Memory();
        GSendBufferManager = new SendBufferManager();
        GDeadLockProfiler = new DeadLockProfiler();
        GGlobalQueue = new GlobalQueue();
        SocketUtils::Init();
    }
    ~CoreGlobal()
    {
        delete GThreadManager;
        delete GMemory;
        delete GSendBufferManager;
        delete GDeadLockProfiler;
        delete GGlobalQueue;
        SocketUtils::Clear();
    }
} GCoreGlobal;


