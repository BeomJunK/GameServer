#pragma once
#include "DBConnection.h"

/*---------------------
	DBConnectionPool
---------------------*/
class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool Connect(int32 connectionCount, const WCHAR* connectionString);
	void Clear();

	//쓰고 바로 반납할거기때문에 레퍼런스관리 안해줌
	DBConnection* Pop();
	void Push(DBConnection* connection);
	
private:
	USE_LOCK;
	SQLHENV _environment = SQL_NULL_HANDLE;
	Vector<DBConnection*> _connections;
};

