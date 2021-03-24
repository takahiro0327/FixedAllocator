# FixedAllocator
A component that allocates/deallocates memory of a fixed size.

## Features

- For embedded systems
- Allocate/Free a fixed size block
- Buffer is provided externally.
- For single-threaded applications. Does not run on multi-threaded systems.
- Very fast. Computational order of Allocate/Free is O(1). 
- Small memory overhead. Only one pointer of memory for management.

## Example
```c++
#include <iostream>
#include <cstdlib>
#include "FixedAllocator.hpp"

int main()
{
    uint8_t buffer[64];

    FixedAllocator allocator;
    
    if( !allocator.Initialize( buffer, sizeof(buffer), 16 ) )
    {
        return -1;
    }
    
    void* buff[5];
    
    for( int loop = 0; loop < 3; ++loop )
    {
        printf("-------------------\n");
        
        for( int i = 0; i < 5; ++i )
        {
            buff[i] = allocator.Allocate();
            if( buff[i] ) memset( buff[i], 0xCC, 16 );
            printf("alloc %d : %p\n", i, buff[i]);
        }
    
        for( int i = 0; i < 5; ++i )
        {
            printf("free  %d : %p\n", i, buff[i]);
            allocator.Free(buff[i]);
        }
    }
    
    return 0;
}

```