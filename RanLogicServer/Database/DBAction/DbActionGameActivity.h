#ifndef _DB_ACTION_GAME_ACTIVITY_H_
#define _DB_ACTION_GAME_ACTIVITY_H_

#pragma once
//
//mjeon.activities
//

#include "DbAction.h"
#include "../../../RanLogic/Activity/ActivityBase.h"
#include "../../../RanLogic/Activity/TitleManager.h"

#include "../../Activity/ActivityManager.h"


//
// user-custom information of mmActivities -----> DB
//
class CActivityLoad : public db::DbAction
{
public:
	CActivityLoad(std::tr1::shared_ptr<ActivityManager> &pActivityMan, std::tr1::shared_ptr<TitleManager> &pTitleMan, UINT nChaNum);
	virtual ~CActivityLoad() {};
	int Execute(NetServer* m_pServer);

protected:

	std::tr1::shared_ptr<ActivityManager>	m_pActivityMan;
	std::tr1::shared_ptr<TitleManager>		m_pTitleMan;

	//MMACTIVITIES	&m_mmActivities;
	//TitleManager	&m_TitleMan;

	UINT			m_nChaNum;
	LONGLONG		*m_pllRewardPoint;
};


//
// DB -----> user-custom information of mmActivities
//
class CActivitySave : public db::DbAction
{
public:
	
	//
	// mmActivities MUST be copied but not referenced because the owner character could be destructed before
	//  CActivitySave is poped and CActivitySave::Execute() runs.
	//
	CActivitySave(DWORD dwClientID, std::tr1::shared_ptr<ActivityManager> &pActivityMan, std::tr1::shared_ptr<TitleManager> &pTitleMan, UINT nChaNum, UINT nChaLevel);
	virtual ~CActivitySave() {};
	int Execute(NetServer* m_pServer);

protected:

	std::tr1::shared_ptr<ActivityManager>	m_pActivityMan;
	std::tr1::shared_ptr<TitleManager>		m_pTitleMan;
	
	UINT			m_nChaNum;
	UINT			m_nChaLevel;
};


//
// Activities based on the order of completion -----> DB transaction
//
class CActivityByOrder : public db::DbAction
{
public:
	CActivityByOrder(DWORD dwClientID, std::tr1::shared_ptr<ActivityManager> &pActivityMan, Activity &actv, UINT nChaNum, UINT nChaLevel);
	virtual ~CActivityByOrder() {};
	int Execute(NetServer* m_pServer);


protected:

	std::tr1::shared_ptr<ActivityManager>	m_pActivityMan;

	Activity		&m_Activity;
	UINT			m_nChaNum;
	UINT			m_nChaLevel;
};



#endif // _DB_ACTION_GAME_ACTIVITY_H_