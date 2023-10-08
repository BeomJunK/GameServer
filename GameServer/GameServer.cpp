#include "pch.h"
#include <iostream>


class Knight
{
public:
	Knight(){ cout << "KNight()" << endl;}
	~Knight(){ cout << "~KNight()" << endl;}

};

int main()
{
	shared_ptr<Knight> k = ObjectPool<Knight>::MakeShared();


}


