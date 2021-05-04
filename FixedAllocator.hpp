#ifndef ___FIXED_ALLOCATOR_HPP___
#define ___FIXED_ALLOCATOR_HPP___

#include <stddef.h>
#include <stdint.h>

class FixedAllocator
{
private:

    uintptr_t m_pHead;

public:

    bool Initialize( void* pBuff, size_t buffSize, size_t blockSize )
	{        
        m_pHead = (uintptr_t)nullptr;
    
        if( pBuff == nullptr || blockSize <= 0 )
            return false;
        
        uintptr_t head = (uintptr_t)pBuff;
        
        if( (head & (sizeof(uintptr_t)-1)) != 0 )
            return false;   //Buffer alignment is inappropriate. Does not have an alignment greater than a pointer.
    
        if( ((blockSize & (sizeof(uintptr_t) - 1))) != 0 )
            return false;   //Block size is inappropriate. Does not have an alignment greater than a pointer.
    
        if( buffSize < blockSize )
            return false;
        
        uintptr_t end = head + buffSize - blockSize;
        m_pHead = head;
        
        while( head < end )
        {
            uintptr_t next = head + blockSize;
            *(uintptr_t*)head = next;
            head = next;
        }
        
        *(uintptr_t*)head = (uintptr_t)nullptr;
        
        return true;
    }

    void* Allocate()
    {
        uintptr_t head = m_pHead;
        
        if( head )
        {
            m_pHead = *(uintptr_t*)head;
        }
    
        return (void*)head;
    }

    void Free( void* p )
    {
        if( p )
        {
            uintptr_t block = (uintptr_t)p;
            *(uintptr_t*)block = m_pHead;
            m_pHead = block;
        }
    }
};

#endif  //___FIXED_ALLOCATOR_HPP___
