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

class Knight
{
public:
    Knight ( )
    {
        cout << "기사 생성" << endl;
    }
    ~Knight ( )
    {
        cout << "기사 소멸" << endl;
    }
private:
    int64 _hp = 10;
};
int main()
{
    for ( int i = 0; i < 5; i++ )
    {
        GThreadManager->Launch ( [ ] ( )
        {
            while ( true )
            {
                Vector<Knight> v ( 10 );

                Map<int32 , Knight> m;
                m [ 100 ] = Knight ( );

                this_thread::sleep_for ( 10ms );
            }
        } );
    }


    GThreadManager->Join ( );
}


