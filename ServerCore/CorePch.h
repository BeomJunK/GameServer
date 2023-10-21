#pragma once
//----------
//	Window, STL
//----------
#include <iostream>
#include <atomic>
#include <mutex>
#include <memory>
#include <future>
#include <Windows.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

//-----------------
//Global Type Macro
//-----------------
#include "Types.h"
#include "CoreMacro.h"
#include "CoreGlobal.h"

//----------
//	Memory
//----------
#include "Allocator.h"
#include "Memory.h"
#include "Container.h"
#include "ObjectPool.h"

//----------
//	Lock
//----------
#include "Lock.h"

//----------
//	Thread
//----------
#include "ThreadManager.h"
#include "CoreTLS.h"


//----------
//	Smart Pointer
//----------
#include "RefCounting.h"




