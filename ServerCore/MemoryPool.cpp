#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool ( int32 allocSize ) : _allocSize ( allocSize )
{
}

MemoryPool::~MemoryPool ( )
{
	while ( _queue.empty ( ) == false )
	{
		MemoryHeader* header = _queue.front ( );
		_queue.pop ( );
		::free ( header );
	}
}

void MemoryPool::Push ( MemoryHeader* ptr )
{
	WRITE_LOCK;

	ptr->allocSize = 0;

	//메모리 반남
	_queue.push ( ptr );
	_allocCount.fetch_sub ( 1 );
}

MemoryHeader* MemoryPool::Pop ( )
{
	MemoryHeader* header = nullptr;

	{
		WRITE_LOCK;

		//pool에 여분이 있다
		if ( _queue.empty() == false )
		{
			//있으면 하나 꺼내옴
			header = _queue.front ( );
			_queue.pop ( );
		}
	}

	//없으면 새로만듬
	if ( header == nullptr )
	{
		header = reinterpret_cast< MemoryHeader* >( ::malloc ( _allocSize ));
	}
	else
	{
		ASSERT_CRASH ( header->allocSize == 0 );
	}

	_allocCount.fetch_add ( 1 );
	return header;
}
