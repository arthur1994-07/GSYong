#ifndef _DB_ACTION_GAME_SNS_H_
#define _DB_ACTION_GAME_SNS_H_

#pragma once

#include "../../../RanLogic/GLogicData.h"
#include "DbAction.h"

//
//mjeon.sns
//


//
// FACEBOOK
//

class CSNSAuthedFB: public db::DbAction
{
public:
	CSNSAuthedFB ( DWORD dwClient, DWORD dwGaeaID, int nChaNum, char *pSKey, char *pUID );
	virtual ~CSNSAuthedFB() {;}
	virtual int Execute(NetServer *pServer);

protected:
	SFACEBOOK	m_AUTHFB;
	DWORD		m_dwGaeaID;
};


class CSNSUnAuthedFB: public db::DbAction
{
public:
	CSNSUnAuthedFB ( DWORD dwClient, DWORD dwGaeaID, int nChaNum );
	virtual ~CSNSUnAuthedFB() {;}
	virtual int Execute(NetServer *pServer);

protected:	
	DWORD		m_dwGaeaID;
	int			m_nChaNum;
};

class CSNSGetUidFB: public db::DbAction
{
public:
	CSNSGetUidFB ( DWORD dwClient, DWORD dwGaeaID, DWORD dwTargetGaeaID, int nTargetChaNum );
	virtual ~CSNSGetUidFB() {;}
	virtual int Execute(NetServer *pServer);

protected:
	std::string		m_UID;
	int				m_TargetChaNum;
	DWORD			m_TargetGaeaID;

	DWORD			m_dwGaeaID;	
};


//
// TWITTER
//

class CSNSAuthedTW: public db::DbAction
{
public:
	CSNSAuthedTW ( DWORD dwClient, DWORD dwGaeaID, int nChaNum, char *pAToken, char *pATokenS, char *pUID );
	virtual ~CSNSAuthedTW() {;}
	virtual int Execute(NetServer *pServer);

protected:
	STWITTER	m_AUTHTW;
	DWORD		m_dwGaeaID;
};


class CSNSUnAuthedTW: public db::DbAction
{
public:
	CSNSUnAuthedTW ( DWORD dwClient, DWORD dwGaeaID, int nChaNum );
	virtual ~CSNSUnAuthedTW() {;}
	virtual int Execute(NetServer *pServer);

protected:	
	DWORD		m_dwGaeaID;
	int			m_nChaNum;
};

class CSNSGetUidTW: public db::DbAction
{
public:
	CSNSGetUidTW ( DWORD dwClient, DWORD dwGaeaID, DWORD dwTargetGaeaID, int nTargetChaNum );
	virtual ~CSNSGetUidTW() {;}
	virtual int Execute(NetServer *pServer);

protected:
	std::string		m_UID;
	int				m_TargetChaNum;
	DWORD			m_TargetGaeaID;

	DWORD			m_dwGaeaID;
};



//
// SNS Common
//

class CSNSGetInfo: public db::DbAction
{
public:
	CSNSGetInfo ( DWORD dwClient, DWORD dwGaeaID, int nChaNum );
	virtual ~CSNSGetInfo() {;}
	virtual int Execute(NetServer *pServer);

protected:
	SFACEBOOK	m_AUTHFB;
	STWITTER	m_AUTHTW;
	DWORD		m_dwGaeaID;	
};

#endif // _DB_ACTION_GAME_SNS_H_