#include "pch.h"
#include "SendBuffer.h"

/* ----------------
      SendBuffer
 ----------------*/
SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize)
    :_owner(owner), _buffer(buffer), _allocSize(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 writeSize)
{
    ASSERT_CRASH(_allocSize >= writeSize);
    _writeSize = writeSize;
    _owner->Close(writeSize);
}
/* ---------------------
      SendBuffeChunk
 --------------------*/
SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
    _open = false;
    _usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(int32 allocSize)
{
    //TLS영역에서 실행되는것이기 때문에 Lock 필요 x
    ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
    ASSERT_CRASH(_open == false);

    if(static_cast<uint32>(allocSize) > FreeSize())
        return nullptr;

    _open = true;

    return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{
    ASSERT_CRASH(_open == true);
    _open = false;
    _usedSize += writeSize;
}
/* ----------------------
      SendBufferManager
 ------------------------*/
SendBufferRef SendBufferManager::Open(uint32 size)
{
    //큰 영역에서 사용할 일부분을 열어준다
    //예약을 해뒀다가 진짜로 쓸 영역은 Close해서 사용
    if(LSendBufferChunk == nullptr)
    {
        LSendBufferChunk = Pop(); //WRITE LOCK
        LSendBufferChunk->Reset();
    }

    ASSERT_CRASH(LSendBufferChunk->IsOpen() == false)

    //다썻으면 버리고 새걸로 교체
    if(LSendBufferChunk->FreeSize() < size)
    {
        LSendBufferChunk = Pop(); //WRITE LOCK
        LSendBufferChunk->Reset();
    }

    cout << "Free Size : " << LSendBufferChunk->FreeSize() << endl;
    
    return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
    cout << "Pop Chunck" << endl;
    {
        WRITE_LOCK
        if(_sendBufferChunks.empty()==false)
        {
            SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
            _sendBufferChunks.pop_back();
            return sendBufferChunk;
        }
    }

    //delete 될때 PushGlobal
    return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
    WRITE_LOCK
    _sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
    cout << "Push Global" << endl;
    GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}
