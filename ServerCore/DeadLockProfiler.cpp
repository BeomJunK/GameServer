#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock ( const char* name )
{
	LockGuard guard ( _lock );

	//id를 찾거나 발급
	int32 lockId = 0;

	auto findIt = _nameToId.find ( name );
	if ( findIt == _nameToId.end ( ) )
	{
		lockId = static_cast< int32 >( _nameToId.size ( ) );
		_nameToId [ name ] = lockId;
		_idToName [ lockId ] = name;
	}
	else
	{
		lockId = findIt->second;
	}

	//잡고있던 락이 있었다면
	if ( LLockStack.empty ( ) == false )
	{
		//기존에 발견되지 않은 케이스라면  데드락 여부 다시 판단.

		//가장 최근에 잡은락 
		const int32 prevId = LLockStack.top ( );
		//재귀적으로 같은락을 잡는건 ok
		//아닌상황에만 체크
		if ( lockId != prevId )
		{
			set<int32>& history = _lockHistory [ prevId ];
			if ( history.find ( lockId ) == history.end ( ) )
			{
				history.insert ( lockId );
				CheckCycle ( );
			}
		}
	}

	LLockStack.push ( lockId );
}

void DeadLockProfiler::PopLock ( const char* name )
{
	LockGuard guard ( _lock );

	if ( LLockStack.empty ( ) )
		CRASH ( "MULTIPLE_UNLOCK" );

	int32 lockId = _nameToId [ name ];
	if ( LLockStack.top ( ) != lockId )
		CRASH ( "INVALID_UNLOCK" );

	LLockStack.pop ( );
}

void DeadLockProfiler::CheckCycle ( )
{
	const int32 lockCount = static_cast< int32 >( _nameToId.size ( ) );
	_discoveredOrder = vector<int32> ( lockCount , -1 );
	_discoveredCount = 0;
	_finished = vector<bool> ( lockCount , false );
	_parent = vector<int32> ( lockCount , -1 );

	for ( int32 lockId = 0; lockId < lockCount; lockId++ )
		Dfs ( lockId );

	//연산끝났으니 정리
	_discoveredOrder.clear ( );
	_finished.clear ( );
	_parent.clear ( );
}

void DeadLockProfiler::Dfs ( int32 here )
{
	//다른애가 이미 방문함
	if ( _discoveredOrder [ here ] != -1 )
		return;

	//방문된 순서 기입 0 -> 1 -> 2
	_discoveredOrder [ here ] = _discoveredCount++;

	//모든인접한 정접 순회
	auto findIt = _lockHistory.find ( here );
	if ( findIt == _lockHistory.end ( ) )
	{
		//락은잡은상태에서 다른락을 잡은적이 없다.
		_finished [ here ] = true;
		return;
	}

	//다른 락을 잡은적이있으니 Cycle 확인
	set<int32> nextSet = findIt->second;
	for ( int32 there : nextSet )
	{
		//아직 방문 한적이 없다면 방문
		if ( _discoveredOrder [ there ] == -1 )
		{
			_parent [ there ] = here;
			Dfs ( there );
			continue;
		}
		//here 가 there보다 먼저 발견되었다면 there는 here의 후손
		//(순방향 간선) 문제없스니 통과
		if ( _discoveredOrder [ here ] < _discoveredOrder [ there ] )
			continue;

		//순방향이 아니고  there는 here의 선조
		if ( _finished [ there ] == false )
		{
			printf ( "%s -> %s\n" , _idToName [ here ] , _idToName [ there ] );

			int32 now = here;
			while ( true )
			{
				printf ( "%s -> %s\n" , _idToName [ _parent [ now ] ] , _idToName [ now ] );
				now = _parent [ now ];
				if ( now == there )
					break;
			}

			CRASH ( "DEADLOCK_DETECTED" );
		}
	}

	_finished [ here ] = true;
}
