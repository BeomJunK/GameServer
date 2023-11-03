#pragma once
#include "Container.h"
#include "Types.h"

class SendBufferChunk;
/* ----------------
      SendBuffer
 ----------------*/
//생각해봐야할 문제
//1. 연속적으로 보내개 된다면 가능한가?
//2. WSASend는 멀티 스레드 환경에서 Safe하지 않다.
//3. 어떤식으로건 순서를 보장 해줘야한다 보내더라도 순서가 섞여버릴수있음
//4. 버퍼가 꽉 찼다면 굳이 데이터를 밀어넣을 필요가 없다.

class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
    SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize);
    ~SendBuffer();

    BYTE* Buffer(){return _buffer; }
    int32 WriteSize() { return _writeSize; }
    void Close(uint32 writeSize);
private:
    BYTE* _buffer;
    uint32 _allocSize = 0;
    int32 _writeSize = 0;
    SendBufferChunkRef _owner; //ref카운팅이 되기때문에 절대 사라지지않을것이다.
};



/* ---------------------
      SendBuffeChunk - TLS영역에서 실행됨
 --------------------*/

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
    enum
    {
        SEND_BUFFER_CHUNK_SIZE = 6000,
    };
public:
    SendBufferChunk();
    ~SendBufferChunk();

    void Reset();
    SendBufferRef Open(int32 allocSize);
    void Close(uint32 writeSize);

    bool IsOpen() { return _open;}
    BYTE* Buffer() { return &_buffer[_usedSize]; }
    uint32 FreeSize() { return static_cast<uint32>(_buffer.size())- _usedSize;}
private:
    Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
    bool _open = false;
    uint32 _usedSize = 0;
};


/* ----------------------
      SendBufferManager
 ------------------------*/
class SendBufferManager
{
public:
    SendBufferRef Open(uint32 size);

private:
    SendBufferChunkRef Pop();
    void Push(SendBufferChunkRef buffer);

    static void PushGlobal(SendBufferChunk* buffer);
    
private:
    USE_LOCK
    Vector<SendBufferChunkRef> _sendBufferChunks;
};
