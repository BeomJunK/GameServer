#pragma once
#include "Allocator.h"

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

