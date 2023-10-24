#pragma once
/*--------------
  NetAddress
 --------------*/

//나중에 클라이언트의 IP주소를 추출해주고 싶은데
//그때 매번 함수를 하나하나 호출 해줄 필요없이 랩핑 해서 사용 한다.
class NetAddress
{
public:
    NetAddress();
    NetAddress(SOCKADDR_IN sockAddr);
    NetAddress(wstring ip, uint16 port);

    SOCKADDR_IN& GetSockAddr(){return _sockAddr;}
    wstring     GetIPAddress();
    uint16      GetPort() { return ::ntohs(_sockAddr.sin_port);}

    static IN_ADDR Ip2Address(const WCHAR* ip);
private:
    SOCKADDR_IN _sockAddr = {};
};

