#pragma once

#include <string>
#include <vector>

struct SSTATION
{
	DWORD		dwMAPID;
	DWORD		dwGATEID;
	std::string	strMAP;
	std::string	strSTATION;
	DWORD		dwLINKID;		// NpcAction 버스리스트에서 참조할 메인 버스리스트의 아이디
	DWORD		dwProbability;  // 확률

	SSTATION () :
		dwMAPID(0),
		dwGATEID(0),
		dwLINKID(0),
		dwProbability(0)
	{
	}

	SSTATION& operator= ( const SSTATION& value )
	{
		dwMAPID = value.dwMAPID;
		dwGATEID = value.dwGATEID;
		strMAP = value.strMAP;
		strSTATION = value.strSTATION;
		dwLINKID = value.dwLINKID;
		dwProbability = value.dwProbability;

		return *this;
	}
};

struct SVILLAGE
{
	DWORD				dwMAPID;
	std::string			strMAP;
	std::vector<DWORD>	vecSTATION;

	SVILLAGE () :
		dwMAPID(0)
	{
	}

	SVILLAGE& operator= ( const SVILLAGE& value )
	{
		dwMAPID = value.dwMAPID;
		strMAP = value.strMAP;
		return *this;
	}

	DWORD GetStationNum ()
	{
		return (DWORD) vecSTATION.size();
	}

	DWORD GetStation ( DWORD dwID )
	{
		if ( vecSTATION.size() <= dwID )
			return UINT_MAX;

		return vecSTATION[dwID];
	}
};

class GLBusData
{
public:
	GLBusData();
	virtual ~GLBusData();

protected:
	std::vector<SSTATION>	m_vecSTATION;

public:
	virtual bool LOAD(const std::string& strFILE, bool bToolMode) = 0;
    virtual SSTATION* GetStation(DWORD dwID) = 0;

	DWORD GetStationNum() const { return (DWORD)m_vecSTATION.size(); }
};