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
    DWORD	m_ClientSlot;	 // �޽����� ���� Ŭ���̾�Ʈ ��ȣ	
    std::string	m_strUserIP; // IP Address
    USHORT	m_uPort;
    IDbManager*	m_pDbManager;
    IDbManager* m_pSubDbManager;	//mjeon.ado

    DB_TYPE m_dbType; //! � DB Thread ���� �����ؾ� �ϴ��� �����Ѵ�.

public:
	//
	//mjeon.ado
	//SubDbMananer: �� ���������� �ϳ�, ���� ODBC�� ó���ο� ������ ���� �����鼭 ADO �ڵ带 ���� �� �ֵ��� �ϱ� �����̴�.
	// ��, �ϳ��� DbAction���� ������ ODBCó���� ���Ҿ� �߰������� �����ϴ� ������ ADO�� ����ϰ� ���� ��쿡 �̸� ����ϸ� �ȴ�.
	//
	virtual void SetDbManager(IDbManager* pDbMan) override;
	virtual void SetSubDbManager(IDbManager* pAdoMan);
	virtual int Execute(NetServer* pServer) override;
    
    //! � DB Thread ���� �����ؾ� �ϴ��� �����Ѵ�.
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