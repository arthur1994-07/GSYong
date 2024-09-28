#ifndef _DB_ACTION_H_
#define _DB_ACTION_H_

#pragma once

#include <boost/tr1/memory.hpp>
#include "../DbDefine.h"

class NetServer;

namespace db
{

class IDbManager;

//
//mjeon.dbman
//
class IDbAction 
{
public:
    IDbAction() {}
    virtual ~IDbAction() {}

public:
	virtual void SetDbManager(IDbManager* pDbMan) = 0;
	virtual int Execute(NetServer* m_pServer) = 0;
};

class DbAction : public IDbAction
{
public:	
	DbAction(DB_TYPE dbType=GAME_DB, DWORD dwClient=-1, USHORT uPort=0);
	virtual ~DbAction();

protected:
    DWORD	m_ClientSlot;	 // 메시지를 보낸 클라이언트 번호	
    std::string	m_strUserIP; // IP Address
    USHORT	m_uPort;
    IDbManager*	m_pDbManager;
    IDbManager* m_pSubDbManager;	//mjeon.ado

    DB_TYPE m_dbType; //! 어떤 DB Thread 에서 실행해야 하는지 결정한다.

public:
	//
	//mjeon.ado
	//SubDbMananer: 좀 억지스럽긴 하나, 기존 ODBC쪽 처리부에 영향을 주지 않으면서 ADO 코드를 겸할 수 있도록 하기 위함이다.
	// 즉, 하나의 DbAction으로 기존의 ODBC처리와 더불어 추가적으로 구현하는 내용은 ADO를 사용하고 싶을 경우에 이를 사용하면 된다.
	//
	virtual void SetDbManager(IDbManager* pDbMan) override;
	virtual void SetSubDbManager(IDbManager* pAdoMan);
	virtual int Execute(NetServer* pServer) override;
    
    //! 어떤 DB Thread 에서 실행해야 하는지 결정한다.
    DB_TYPE Type() const { return m_dbType; }
};

typedef std::tr1::shared_ptr<DbAction> DbActionPtr;

class DbActionNull : public DbAction
{
public:
    static const DbActionPtr Null;
};

} // namespace db
#endif // _DB_ACTION_H_