#include "pch.h"


ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;

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
        GJobTimer = new JobTimer();
        GDBConnectionPool = new DBConnectionPool();

        SocketUtils::Init();
    }
    ~CoreGlobal()
    {
        delete GThreadManager;
        delete GMemory;
        delete GSendBufferManager;
        delete GGlobalQueue;
        delete GJobTimer;
        delete GDeadLockProfiler;
        delete GDBConnectionPool;
        SocketUtils::Clear();
    }
} GCoreGlobal;


