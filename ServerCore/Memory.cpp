#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

Memory::Memory ( )
{
	int32 size = 0;
	int32 tableIndex = 0;

	for ( size = 32; size < 1024; size += 32 )
	{
		MemoryPool* pool = new MemoryPool( size );
		_pools.push_back ( pool );

		while ( tableIndex <= size )
		{
			_poolTable [ tableIndex ] = pool;
			tableIndex++;
		}
	}


	for (size = 1024; size < 2048; size += 128 )
	{
		MemoryPool* pool = new MemoryPool ( size );
		_pools.push_back ( pool );

		while ( tableIndex <= size )
		{
			_poolTable [ tableIndex ] = pool;
			tableIndex++;
		}
	}


	for (size = 2048; size <= 4096; size += 256 )
	{
		MemoryPool* pool = new MemoryPool ( size );
		_pools.push_back ( pool );

		while ( tableIndex <= size )
		{
			_poolTable [ tableIndex ] = pool;
			tableIndex++;
		}
	}

	{
		int a = 0;
	}
}

Memory::~Memory ( )
{
	for ( MemoryPool* pool : _pools )
		delete pool;

	_pools.clear ( );
}

void* Memory::Allocate ( int32 size )
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof ( MemoryHeader );

#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
#else
	if ( allocSize > MAX_ALLOC_SIZE )
	{
		//메모리 풀링 최대크기 넘어가면 일반 할당
		//메모리 헤더를 붙이기 전상태임
		header = reinterpret_cast< MemoryHeader* >( ::_aligned_malloc ( allocSize, SLIST_ALIGNMENT) );
	}
	else
	{
		//메모리 풀에서 꺼내온다
		//메모리 헤더를 붙이기 전상태임
		header = _poolTable [ allocSize ]->Pop();
	}
#endif

	//메모리 헤더를 붙이기
	return MemoryHeader::AttachHeader ( header , allocSize );
}

void Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	const int32 allocSize = header->allocSize;
	ASSERT_CRASH ( allocSize > 0 );

	if ( allocSize > MAX_ALLOC_SIZE )
	{
		//최대치보다 크면 일반 해제
		::_aligned_free ( header );
		return;
	}

	_poolTable [ allocSize ]->Push ( header );
#endif
}
