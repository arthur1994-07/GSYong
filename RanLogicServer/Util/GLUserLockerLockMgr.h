#pragma once

#include <string>
#include <vector>

#include "../../SigmaCore/Database/Sqlite/SQLite.h"

namespace db
{
	class IDbManager;
}

class GLUserLockerLockMgr
{
public:
	GLUserLockerLockMgr( db::IDbManager* pDBMan );
	~GLUserLockerLockMgr();

	enum eCommonError
	{
		eSuccess = 0, 
		eChaIsNull, 
		eExecute, 
		eOpen, 
		ePrepare, 
		eBind, 
		eStep, 
		eStringEmpty, 
		eWrongItem, 
		eNullTablePointer, 
		eDBManNull, 
		eCompress, 
		eDecompress, 
	};

public:
	void Lock( DWORD UserNum );
	void UnLock( DWORD UserNum );
	void UnLock();

protected:
	int open();
	void close();
	int executeSQL( const std::string& strQuery );

protected:
	db::IDbManager* m_pAdoMan;
	SQLite::Database m_UserLockerLock;

private:

};
