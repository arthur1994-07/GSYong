#pragma once

#include "../../ProvideRewardSystem/GLProvideRewardManager.h"

#include "DbAction.h"

// Initialize;
class CProvideRewardInitializeAction : public db::DbAction
{
public:
	CProvideRewardInitializeAction ( );
	virtual ~CProvideRewardInitializeAction ( );

public:
	virtual int Execute ( NetServer* pServer ) override;
};


// Insert;
class CProvideRewardInsertAction : public db::DbAction
{
public:
	CProvideRewardInsertAction (
		const ProvideRewardSystem::PROVIDE_REWARD_DATA& _ProvideRewardData );
	virtual ~CProvideRewardInsertAction ( );

public:
	virtual int Execute ( NetServer* pServer ) override;

private:
	ProvideRewardSystem::PROVIDE_REWARD_DATA m_ProvideRewardData;
};


// Select;
class CProvideRewardSelectAction : public db::DbAction
{
public:
	CProvideRewardSelectAction ( DWORD _dwChaDBNum );
	virtual ~CProvideRewardSelectAction ( );

public:
	virtual int Execute ( NetServer* pServer ) override;

private:
	DWORD m_dwChaDBNum;
	ProvideRewardSystem::VEC_PROVIDE_REWARD m_vecProvideRewardData;
};


// Clear Delete Flag;
class CProvideRewardClearDeleteFlagAction : public db::DbAction
{
public:
	CProvideRewardClearDeleteFlagAction ( );
	virtual ~CProvideRewardClearDeleteFlagAction ( );

public:
	virtual int Execute ( NetServer* pServer ) override;
};


// Complete;
class CProvideRewardCompleteAction : public db::DbAction
{
public:
	CProvideRewardCompleteAction ( DWORD _dwUniqueID );
	virtual ~CProvideRewardCompleteAction ( );

public:
	virtual int Execute ( NetServer* pServer ) override;

private:
	DWORD m_dwUniqueID;
};