//mjeon.instance.match

#pragma once

#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"

#include <vector>


enum REQUEST_TYPE
{
	REQUEST_MATCH	= 0,
	REQUEST_UNMATCH	= 1,
};

enum REQUEST_CONTENT
{
	REQUEST_PVE		= 0,
	REQUEST_PVP		= 1,
};

enum INSTANCE_CREATE_CODE
{
	INSTANCE_CREATE_DONE			= 0,
	INSTANCE_CREATE_ALREADY_EXIST	= 1,
	INSTANCE_CREATE_WRONG_MAPID		= 2,
};

enum INSTANCE_DELETE_CODE
{
	INSTANCE_DELETE_DONE			= 0,
	INSTANCE_DELETE_NOT_EXIST		= 1,
	INSTANCE_DELETE_WRONG_MAPID		= 2,
};

/*
network msg for instance match request

ex>

struct MATCH_CHAINFO
{
	DWORD dwChaNum;
	DWORD dwClass;
	DWORD dwLevel;
};

struct NET_MSG_MATCH
{
	DWORD dwSvrGroup;
	DWORD dwMapID;		// 어느 인던으로?
	DWORD dwPartySize;	// 1 ~ MAX_PARTY_SIZE

	MATCH_CHAINFO arrParty[MAX_PARTY_SIZE];
};
*/


struct MATCH_CHAINFO
{
	DWORD dwChaNum;
	DWORD dwClass;
	DWORD dwLevel;
};


//
//Matching의 주체(Character)에 대한 추상계층
//
struct INSTANCE_PLAYER : public UNIQUEID
{
	INSTANCE_PLAYER()
		:dwClass(0)
		,dwLevel(0)
	{
	}

	INSTANCE_PLAYER(DWORD _dwSvrGroupNum, DWORD _dwChaNum)
		:UNIQUEID(_dwSvrGroupNum, _dwChaNum)
	{
	}

	INSTANCE_PLAYER(bool bValid)
		:UNIQUEID(bValid)
		,dwClass(0)
		,dwLevel(0)
	{
	}

	virtual ~INSTANCE_PLAYER()
	{
	}

	bool operator == (const INSTANCE_PLAYER& rhs) const
    {
        return ((dwSvrGroup == rhs.dwSvrGroup) && (dwChaNum == rhs.dwChaNum));
    }

    bool operator != (const INSTANCE_PLAYER& rhs) const
    {
        return !((dwSvrGroup == rhs.dwSvrGroup) && (dwChaNum == rhs.dwChaNum));
    }

    bool operator < (const INSTANCE_PLAYER& rhs) const
    {
        return (dw64id < rhs.dw64id);
    }

	DWORD		dwClass;	//unique job identity including gender
	DWORD		dwLevel;
};


typedef std::vector<INSTANCE_PLAYER>	VEC_INSTANCE_PLAYER;

//
//Matching Request의 기본 단위
//
struct MATCH_REQUEST
{
	MATCH_REQUEST()
		:dwMapID(INVALID_MAP_ID)
		,dwKey(INVALID_INSTANCE_KEY)
		,Type(REQUEST_MATCH)
	{
		m_vecParty.clear();
	}

	MATCH_REQUEST(DWORD _dwMapID)
		:dwMapID(_dwMapID)
		,dwKey(INVALID_INSTANCE_KEY)
		,Type(REQUEST_MATCH)
	{
		m_vecParty.clear();
	}

	/*
	MATCH_REQUEST(NET_MSG_MATCH *msg)
	{
		msg->dwSvrGroup;
		msg->dwPartySize;
		msg->arrParty[]

		를 사용해서 셋업
	}
	*/

	virtual ~MATCH_REQUEST()
	{
	}

	void AddMember(INSTANCE_PLAYER &member)
	{
		m_vecParty.push_back(member);
	}

	INSTANCE_PLAYER& GetMember(DWORD idx)
	{
		GASSERT(idx < size());

		if (idx >= size())
		{
			sc::writeLogError(sc::string::format("MATCH_REQUEST::GetMember() - idx(%1%) is exceeds the size(%2%).", idx, size()));
			return invalid_player;
		}

		return m_vecParty[idx];	//Note: idx 0부터 Matching 기준을 체크하므로, 파티장이 0번 index에 삽입되도록 하자.
	}

	void SetMapID(DWORD _dwMapID)
	{
		dwMapID = _dwMapID;
	}

	DWORD GetMapID()
	{
		return dwMapID;
	}

	void SetKey(DWORD _dwKey)
	{
		dwKey = _dwKey;
	}

	DWORD GetKey()
	{
		return dwKey;
	}

	void SetType(REQUEST_TYPE _Type)
	{
		Type = _Type;
	}

	REQUEST_TYPE GetType()
	{
		return Type;
	}

	DWORD size()
	{
		return (DWORD)m_vecParty.size();
	}

	static INSTANCE_PLAYER invalid_player;

protected:
	
	DWORD			dwMapID;
	DWORD			dwKey;		//UnMatch용
	REQUEST_TYPE	Type;


	VEC_INSTANCE_PLAYER	m_vecParty;	//vector of other members of the party if the match request is for a party.
};

