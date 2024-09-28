#pragma once

#include "DbAction.h"

#include "../../Country/GLCountryManServer.h"

// Update User Country Action;
class CUpdateUserCountryAction : public db::DbAction
{
public:
	CUpdateUserCountryAction ( DWORD dwUserID, Country::SCOUNTRY_INFO sCountryInfo );
	virtual ~CUpdateUserCountryAction ();

	virtual int Execute ( NetServer* pServer ) override;

private:
	DWORD m_dwUserID;
	Country::SCOUNTRY_INFO m_sCountryInfo;
};


// Load Victorious Country Action;
class CLoadAgentVictoriousCountryAction : public db::DbAction
{
public:
	CLoadAgentVictoriousCountryAction ();
	virtual ~CLoadAgentVictoriousCountryAction ();

	virtual int Execute ( NetServer* pServer ) override;

private:
	Country::VCOUNTRY_VEC m_vecVCountry;
};

class CLoadFieldVictoriousCountryAction : public db::DbAction
{
public:
	CLoadFieldVictoriousCountryAction ();
	virtual ~CLoadFieldVictoriousCountryAction ();

	virtual int Execute ( NetServer* pServer ) override;

private:
	Country::VCOUNTRY_VEC m_vecVCountry;
};


// Add Victorious Country Action;
class CAddVictoriousCountryAction : public db::DbAction
{
public:
	CAddVictoriousCountryAction ( Country::SVICTORIOUS_COUNTRY sVCountry );
	virtual ~CAddVictoriousCountryAction ();

	virtual int Execute ( NetServer* pServer ) override;

private:
	Country::SVICTORIOUS_COUNTRY m_sVCountry;
};


// Reset Victorious Country Action;
class CResetVictoriousCountryAction : public db::DbAction
{
public:
	CResetVictoriousCountryAction ( Country::EMCOUNTRY_SYSTEM_FLAG emFlag );
	virtual ~CResetVictoriousCountryAction ();

	virtual int Execute ( NetServer* pServer ) override;

private:
	Country::EMCOUNTRY_SYSTEM_FLAG m_emFlag;
};


// Confirm State Victorious Country Action;
// Game Server -> World Battle Server DB;
class CConfirmStateVictoriousCountryAction : public db::DbAction
{
public:
	CConfirmStateVictoriousCountryAction ( Country::EMCOUNTRY_SYSTEM_FLAG emFlag );
	virtual ~CConfirmStateVictoriousCountryAction ();

	virtual int Execute ( NetServer* pServer ) override;

private:
	Country::EMCOUNTRY_SYSTEM_FLAG m_emFlag;

	// 검색 결과;
	Country::VCOUNTRY_VEC m_vecVCountry;
};