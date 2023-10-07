#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <memory>
#include <windows.h>
#include <future>
#include "ThreadManager.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

int main()
{
	for ( int i = 0; i < 3; i++ )
	{
		GThreadManager->Launch ( [ ] ( )
		{
			
		} );
	}


	GThreadManager->Join ( );
}


