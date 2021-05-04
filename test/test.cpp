#include <gtest/gtest.h>
#include "../FixedAllocator.hpp"

TEST( FixedAllocator_InitializeTest, Default){
  FixedAllocator fa;
  void* buffer[4];

  for( int i = 1; i <= 4; ++i )
  {
    ASSERT_TRUE( fa.Initialize(&buffer[0], sizeof(buffer), sizeof(void*) * i) );
  }
}

TEST(FixedAllocator_InitializeTest, OverBlocksize ){
  FixedAllocator fa;
  void* buffer[4];
  ASSERT_FALSE( fa.Initialize(&buffer[0], sizeof(buffer), sizeof(void*)*5) );
}

TEST(FixedAllocator_InitializeTest, NotAligned1){
  FixedAllocator fa;
  void* buffer[4];
  ASSERT_FALSE( fa.Initialize(&buffer[0], sizeof(buffer), sizeof(void*) - 1) );
}

TEST(FixedAllocator_InitializeTest, NotAligned2){
  FixedAllocator fa;
  void* buffer[4];
  ASSERT_FALSE( fa.Initialize((void*)((uintptr_t)&buffer[0]-1), sizeof(buffer), sizeof(void*)) );
}

TEST(FixedAllocator_Test, Default1){
  FixedAllocator fa;

  constexpr size_t n = 64;
  uint64_t buffer[n];

  for( size_t blockSize = sizeof(uint64_t); blockSize <= sizeof(uint64_t) * n; blockSize += sizeof(uint64_t) )
  {
    for( size_t blockCount = 1; blockCount <= sizeof(buffer) / blockSize; ++blockCount )
    {
      FixedAllocator fa;
      ASSERT_TRUE( fa.Initialize(&buffer[0], blockSize * blockCount, blockSize) );

      void* allocated[n];

      for( size_t loop = 0; loop < 5; ++loop )
      {
        uint8_t random = (uint8_t)rand();

        for( size_t i = 0; i < blockCount; ++i )
        {
          allocated[i] = fa.Allocate();
          ASSERT_NE(allocated[i], nullptr);
          memset( allocated[i], i + random, sizeof(uint64_t) );
          ASSERT_LE((uintptr_t)&buffer[0], (uintptr_t)allocated[i]);
          ASSERT_LE((uintptr_t)allocated[i], (uintptr_t)&buffer[n-1]);
        }

        for( int i = 0; i < 3; ++i )
        {
          ASSERT_EQ( fa.Allocate(), nullptr );
        }

        for( size_t i = 0; i < blockCount; ++i )
        {
          for( uint8_t* p = (uint8_t*)allocated[i], *end = p + sizeof(uint64_t); p != end; ++p )
          {
            ASSERT_EQ( *p, (uint8_t)(i + random) );
          }
          
          fa.Free(allocated[i]);
        }
      }
    }
  }
}




