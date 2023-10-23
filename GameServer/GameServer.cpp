#include "pch.h"



int main()
{
	 SOCKET socket = SocketUtils::CreateSocket();

	SocketUtils::BindAnyAddress(socket, 7777);

	SocketUtils::Listen(socket);

	::accept(socket, nullptr, nullptr);

	cout << "connect client" << endl;

	while (true)
	{

	}
}


