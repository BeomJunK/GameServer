#pragma once
#include "Allocator.h"

class MemoryPool;
/*-----------
 MemoryPool
 *----------*/
class Memory
{
    enum
    {
        //~1024까지 32단위, ~2048 128 단위. ~4096 256 단위
        POOL_COUNT = ( 1024 / 32 ) + ( 1024 / 128 ) + ( 2048 / 256 ) ,
        MAX_ALLOC_SIZE = 4096
    };
public:
    Memory ( );
    ~Memory ( );

    void* Allocate ( int32 size );
    void Release ( void* ptr );

private:
    vector<MemoryPool*> _pools;

    MemoryPool* _poolTable [ MAX_ALLOC_SIZE + 1 ];
};


template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
    //메모리 어떤방식으로 던지 할당 받음
    Type* memory = static_cast<Type*>(xmalloc(sizeof(Type)));

    //placement new
    //일단 메모리에 객체있으닌까 생성자를 호출해달라
    new(memory)Type(std::forward<Args>(args)...);
    
    return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
    obj->~Type();
    PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
    return shared_ptr<Type>{ xnew<Type>(std::forward<Args>(args)...), xdelete<Type> };
}