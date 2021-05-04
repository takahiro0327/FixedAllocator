#include <gtest/gtest.h>
#include "../FixedAllocator.hpp"
#include <algorithm>
#include <random>

TEST( FixedAllocator_InitializeTest, Default)
{
  FixedAllocator fa;
  void* buffer[4];

  for( int i = 1; i <= 4; ++i )
  {
    ASSERT_TRUE( fa.Initialize(&buffer[0], sizeof(buffer), sizeof(void*) * i) );
  }
}

TEST(FixedAllocator_InitializeTest, OverBlocksize )
{
  FixedAllocator fa;
  void* buffer[4];
  ASSERT_FALSE( fa.Initialize(&buffer[0], sizeof(buffer), sizeof(void*)*5) );
}

TEST(FixedAllocator_InitializeTest, NotAligned1)
{
  FixedAllocator fa;
  void* buffer[4];
  ASSERT_FALSE( fa.Initialize(&buffer[0], sizeof(buffer), sizeof(void*) - 1) );
}

TEST(FixedAllocator_InitializeTest, NotAligned2)
{
  FixedAllocator fa;
  void* buffer[4];
  ASSERT_FALSE( fa.Initialize((void*)((uintptr_t)&buffer[0]-1), sizeof(buffer), sizeof(void*)) );
}

bool all_of( void* p, uint8_t value, size_t size )
{
  return std::all_of((uint8_t*)p, (uint8_t*)p + size, [value]( uint8_t x ){ return x == value; });
}

TEST(FixedAllocator_Test, Default1)
{
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
          ASSERT_TRUE( all_of(allocated[i], i+random, sizeof(uint64_t)) );
          fa.Free(allocated[i]);
        }
      }
    }
  }
}

TEST(FixedAllocator_Test, Random)
{
  constexpr size_t n = 128;
  uint64_t buffer[n];

  FixedAllocator fa;
  ASSERT_TRUE( fa.Initialize(&buffer[0], sizeof(buffer), sizeof(uint64_t)) );

  std::vector<void*> allocated;
  std::map<void*,uint8_t> fillValueMap;

  std::random_device seed;
  std::mt19937 engine(seed());
  std::uniform_int_distribution<> dist(0, 99);

  for( int loop = 0; loop < (10 << 10); ++loop )
  {
    if( allocated.size() <= 0 || dist(engine) < 60 )
    {
      void* p = fa.Allocate();

      if( p == nullptr )
      {
        ASSERT_EQ( allocated.size(), n );

        for( size_t i = 0; i < n / 2; ++i )
        {
          size_t freeIndex = std::uniform_int_distribution<>(0,allocated.size()-1)(engine);
          void* p = allocated[freeIndex];

          auto find = fillValueMap.find(p);
          ASSERT_TRUE( find != fillValueMap.end() );

          ASSERT_TRUE( all_of(p, find->second, sizeof(uint64_t)) );
          fa.Free(p);

          fillValueMap.erase(find);
          allocated.erase( allocated.begin() + freeIndex );
        }
      }
      else
      {
        uint8_t fillValue = std::uniform_int_distribution<>(0,255)(engine);
        fillValueMap[p] = fillValue;
        memset( p, fillValue, sizeof(uint64_t) );
        allocated.push_back(p);
      }
    }
    else
    {
      size_t freeIndex = std::uniform_int_distribution<>(0,allocated.size()-1)(engine);
      void* p = allocated[freeIndex];

      auto find = fillValueMap.find(p);
      ASSERT_TRUE( find != fillValueMap.end() );

      ASSERT_TRUE( all_of(p, find->second, sizeof(uint64_t)) );
      fa.Free(p);

      fillValueMap.erase(find);
      allocated.erase( allocated.begin() + freeIndex );
    }
  }

}



