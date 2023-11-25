#pragma once
#include "sql.h"
#include "sqlext.h"

enum
{
    WVARCHAR_MAX = 4000,
    BINARY_MAX = 8000,
};
/* -------------------
      DBConnection
 ------------------- */
class DBConnection
{
public:
    bool Connect(SQLHENV henv, const WCHAR* connectionString);
    void Clear();

    bool Execute(const WCHAR* query); //뭔가 실행
    bool Fetch(); //데이터 긁어오기
    int32 GetRowCount(); //몇줄이냐
    void UnBind(); //바인드 밀어주기

public:
    //cType , sqlType: DB에전달할 데이터타입 msdn 참조
    //https://learn.microsoft.com/ko-kr/sql/odbc/reference/appendixes/c-data-types?view=sql-server-ver16
    bool BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index); //인자 넘겨줄때
    bool BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index); // 인자 받아올떄
    void HandleError(SQLRETURN ret); //에러 처리

    //사용 하기 편하게 인터페이스제공
public:
    bool BindParam(int32 paramIndex, bool* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, float* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, double* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, int8* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, int16* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, int32* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, int64* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, const WCHAR* value, SQLLEN* index);
    bool BindParam(int32 paramIndex, const BYTE* value, int32 size, SQLLEN* index);

    bool BindCol(int32 columnIndex, bool* value, SQLLEN* index);
    bool BindCol(int32 columnIndex, float* value, SQLLEN* index);
    bool BindCol(int32 columnIndex, double* value, SQLLEN* index);
    bool BindCol(int32 columnIndex, int8* value,  SQLLEN* index);
    bool BindCol(int32 columnIndex, int16* value, SQLLEN* index);
    bool BindCol(int32 columnIndex, int32* value, SQLLEN* index);
    bool BindCol(int32 columnIndex, int64* value, SQLLEN* index);
    bool BindCol(int32 columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
    bool BindCol(int32 columnIndex, WCHAR* str, int32 size, SQLLEN* index);
    bool BindCol(int32 columnIndex, BYTE* value, int32 size, SQLLEN* index);
    
private:
    SQLHDBC _connection = SQL_NULL_HANDLE; //data base connection
    SQLHSTMT _statement = SQL_NULL_HANDLE;
};

