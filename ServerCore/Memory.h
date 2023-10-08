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
    Type* memory = static_cast<Type*>(xmalloc(sizeof(Type)));

    //placement new
    //일단 메모리는 있으닌까 메모리위에 생성자를 호출해달라
    new(memory)Type(std::forward<Args>(args)...);
    
    return memory;
}
template<typename Type>
void xdelete(Type* obj)
{
    obj->~Type();
    xrelease(obj);
}

template <typename Type>
shared_ptr<Type> MakeShared()
{
    return make_shared<Type>( xnew<Type>(), xdelete<Type>);
}
