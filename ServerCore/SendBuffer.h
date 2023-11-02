#pragma once

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
    SendBuffer(int32 bufferSize);
    ~SendBuffer();

    BYTE* Buffer(){return _buffer.data(); }
    int32 Capacity() { return static_cast<int32>(_buffer.size()); }

    int32 WriteSize() { return _writeSize; }
    void CopyData(void* data, int32 len);
private:
    Vector<BYTE> _buffer;
    int32 _writeSize = 0;
};

