#include "pch.h"
#include "RecvBuffer.h"
/* -----------
  RecvBuffer
-------------*/
RecvBuffer::RecvBuffer(int32 bufferSize)
{
  //버퍼를 한번에 못받으면 복사 비용이 발생하니 처음부터 크게 잡아준다.
  _capacity = _bufferSize * BUFFER_COUNT;
  _bufferSize = bufferSize;
  _buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
  int32 dataSize = DataSize();
  if(dataSize == 0)
  {
    //다 읽었다.
    _readPos = 0;
    _writePos = 0;
  }
  else
  {
    //여유공간 없을때 데이터 처음으로 밀기
    if(FreeSize() < _bufferSize)
    {
      ::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
      _readPos = 0;
      _writePos = dataSize;
    }
  }

  
  
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
  if(numOfBytes > DataSize())
    return false;

  _readPos += numOfBytes;
  return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
  if(numOfBytes > FreeSize())
    return false;

  _writePos += numOfBytes;
  return true;
}
