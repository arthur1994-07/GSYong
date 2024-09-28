#include "pch.h"
#include "./MatchSystem.h"
#include "../../SigmaCore/gassert.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


MatchingRoom::MatchingRoom(MatchSystem *pMatchSystem, DWORD dwKey, DWORD dwMapId, DWORD dwTO, BOOL bBound, DWORD dwExpireM)
:m_pMatchSystem(pMatchSystem)
,m_dwKey(dwKey)
,m_dwMapID(dwMapId)
,m_dwMaxTO(dwTO)
,m_bBound(bBound)
,m_state(ROOM_STATE_READY)
,m_dwInstanceServerNum(0)
,m_dwExpireM(dwExpireM)
{
	GASSERT(m_pMatchSystem != NULL);
	m_ctCreateTime = CTime::GetCurrentTime();
}

MatchingRoom::MatchingRoom()
:m_pMatchSystem(NULL)
,m_dwKey(INVALID_INSTANCE_KEY)
,m_dwMapID(INVALID_MAP_ID)
,m_bBound(FALSE)
,m_state(ROOM_STATE_READY)
,m_dwInstanceServerNum(0)
,m_dwExpireM(0)
{
	m_ctCreateTime = CTime::GetCurrentTime();
}

MatchingRoom::~MatchingRoom()
{
}

DWORD _KEY_ MatchingRoom::Join(DWORD dwKey, INSTANCE_PLAYER &who, DWORD dwMapID)
{
	if (IsValid() == FALSE)
		return INVALID_INSTANCE_KEY;
	
	GASSERT(m_dwMapID == dwMapID);
	GASSERT(m_dwKey != INVALID_INSTANCE_KEY);
	GASSERT(m_dwKey == dwKey);

	if (m_dwMapID != dwMapID)
	{
		sc::writeLogError(stringformat("MatchingRoom::Join() - MapID(%1%, %2%) is different from the request(%3%, %4%)!!", LOWORD(m_dwMapID), HIWORD(m_dwMapID), LOWORD(dwMapID), HIWORD(dwMapID)));
		
		return INVALID_INSTANCE_KEY;
	}

	if (m_dwKey == INVALID_INSTANCE_KEY)
	{
		sc::writeLogError(stringformat("MatchingRoom::Join() - Room(mapID: %1%, %2%) has Invalid instanceKey!!", SNATIVEID(m_dwMapID).Mid(), SNATIVEID(m_dwMapID).Sid()));
		
		return INVALID_INSTANCE_KEY;
	}

	if (m_dwKey != dwKey)
	{
		sc::writeLogError(stringformat("MatchingRoom::Join() - Key(%1%, %2%) is different from the request(%3%, %4%)!!", m_dwKey, dwKey));
		
		return INVALID_INSTANCE_KEY;
	}

	m_lstPlayers.push_back(who);

	if (m_bBound)
	{
		Bind(who);
	}

	//
	//이미 생성된 room으로 Join하는 경우는 해당 Player를 전송하라
	//  (아닌 경우는 새롭게 생성될 room으로 Join하는 것이다. 이 때, room이 생성될 때 party를 일괄 전송한다.)
	//
	if (m_state == ROOM_STATE_CONSTRUCT)
		BringPlayer(who);

	return m_dwKey;
}

DWORD _KEY_ MatchingRoom::Quit(INSTANCE_PLAYER &who)
{
	if (IsValid() == FALSE)
		return INVALID_INSTANCE_KEY;

	LIST_INSTANCE_PLAYER_ITER iter;

	for (iter = m_lstPlayers.begin(); iter != m_lstPlayers.end(); ++iter)
	{
		if (*iter == who)
		{
			m_pMatchSystem->ResetKey(who);

			m_lstPlayers.erase(iter);

			return m_dwKey;
		}
	}

	return INVALID_INSTANCE_KEY;
}

void MatchingRoom::QuitAll()
{
	if (IsValid() == FALSE)
		return;

	LIST_INSTANCE_PLAYER_ITER iter;

	for (iter = m_lstPlayers.begin(); iter != m_lstPlayers.end(); ++iter)
	{
		m_pMatchSystem->ResetKey(*iter);

		m_lstPlayers.erase(iter);
	}
}

DWORD _KEY_	MatchingRoom::Bind(INSTANCE_PLAYER &who)
{
	if (IsValid() == FALSE)
		return INVALID_INSTANCE_KEY;
	
	return m_pMatchSystem->Bind(m_dwKey, who);
}

void MatchingRoom::BindAll()
{
	if (IsValid() == FALSE)
		return;

	LIST_INSTANCE_PLAYER_ITER iter;

	for (iter = m_lstPlayers.begin(); iter != m_lstPlayers.end(); ++iter)
	{
		INSTANCE_PLAYER &who = *iter;

		Bind(who);
	}
}

void MatchingRoom::BringParty()
{
	if (IsValid() == FALSE)
		return;

	if (m_lstPlayers.size() <= 0)
	{
		sc::writeLogError(stringformat("MatchingRoom::BringParty - The room(%1%) is empty!", m_dwKey));

		return;
	}

	GLMSG::NET_TRANSFER_START_MA msg;	
	msg.dwKey			= m_dwKey;
	msg.dwMapID			= m_dwMapID;
	msg.dwGateID		= INSTANCE_START_GATE;		//Script에서 읽어오도록 수정하자.
	msg.dwInstanceServerNum	= m_dwInstanceServerNum;

	LIST_INSTANCE_PLAYER_ITER iter;

	for (iter = m_lstPlayers.begin(); iter != m_lstPlayers.end(); ++iter)
	{
		INSTANCE_PLAYER &who = *iter;
		
		msg.dwSvrGroupNum	= who.dwSvrGroup;
		msg.dwChaNum		= who.dwChaNum;

		m_pMatchSystem->SendAgent(&msg);
	}	
}

void MatchingRoom::BringPlayer(INSTANCE_PLAYER &_who)
{
	if (IsValid() == FALSE)
		return;

	if (m_lstPlayers.size() <= 0)
	{
		sc::writeLogError(stringformat("MatchingRoom::BringPlayer - The room(%1%) is empty!", m_dwKey));

		return;
	}

	GLMSG::NET_TRANSFER_START_MA msg;	
	msg.dwKey			= m_dwKey;
	msg.dwMapID			= m_dwMapID;
	msg.dwGateID		= INSTANCE_START_GATE;		//Script에서 읽어오도록 수정하자.
	msg.dwInstanceServerNum	= m_dwInstanceServerNum;

	LIST_INSTANCE_PLAYER_ITER iter;

	for (iter = m_lstPlayers.begin(); iter != m_lstPlayers.end(); ++iter)
	{
		INSTANCE_PLAYER &who = *iter;

		if (_who == who)
		{
			msg.dwSvrGroupNum	= who.dwSvrGroup;
			msg.dwChaNum		= who.dwChaNum;

			m_pMatchSystem->SendAgent(&msg);

			break;
		}
	}

	if (iter == m_lstPlayers.end())
	{
		sc::writeLogError(stringformat("MatchingRoom::BringPlayer - The player(%1%, %2%) does not exist in the room!", _who.dwSvrGroup, _who.dwChaNum));
	}
}

void MatchingRoom::KickOutParty()
{
	if (IsValid() == FALSE)
		return;

	if (m_lstPlayers.size() <= 0)
	{
		sc::writeLogInfo(stringformat("MatchingRoom::KickOutParty - The room(%1%) is already empty", m_dwKey));

		return;
	}

	LIST_INSTANCE_PLAYER_ITER iter;

	for (iter = m_lstPlayers.begin(); iter != m_lstPlayers.end(); ++iter)
	{
		INSTANCE_PLAYER &who = *iter;

		KickOutPlayer(who);
	}
}

void MatchingRoom::KickOutPlayer(INSTANCE_PLAYER &who)
{
	//Start transfer the character from the InstanceServer to FieldServer

	GLMSG::NET_TRANSFER_BACK_MI msg;
	msg.dwSvrGroupNum	= who.SvrGroup();
	msg.dwChaNum		= who.ChaDbNum();
	msg.dwKey			= GetKey();
	msg.dwInstanceServerNum	= GetInstanceServerNum();

	m_pMatchSystem->SendInstance(GetInstanceServerNum(), &msg);
}

void MatchingRoom::CheckCancel()
{
	LIST_INSTANCE_PLAYER_ITER iter;

	for (iter = m_lstPlayers.begin(); iter != m_lstPlayers.end();)
	{
		INSTANCE_PLAYER &who = *iter;

		if (m_pMatchSystem->IsCanceled(who.dw64id))
		{
			sc::writeLogDebug(stringformat("Match request of the char(%1%, %2%) has been canceled", who.dwSvrGroup, who.dwChaNum));

			GLMSG::NET_MATCH_CANCEL_CAM_FB msgCancel;
			msgCancel.dwSvrGroupNum = who.dwSvrGroup;
			msgCancel.dwChaNum		= who.dwChaNum;

			m_pMatchSystem->SendAgent(&msgCancel);

			iter = m_lstPlayers.erase(iter);

			continue;
		}
		
		++iter;
	}
}

BOOL MatchingRoom::IsExpired()
{
	GASSERT(CTime::GetCurrentTime() >= m_ctCreateTime);

	if (m_bBound)
	{
		CTimeSpan ctsAge = CTime::GetCurrentTime() - m_ctCreateTime;

		DWORD dwExpireM = m_dwExpireM;
		
		if (dwExpireM == 0)
		{
			dwExpireM = m_pMatchSystem->GetGlobalExpireM();	//해당 인스턴스의 expireM이 없을 경우에는 Global값을 사용
		}

		CTimeSpan ctsLifeTime = CTimeSpan(0, 0, dwExpireM, 0);

		return (ctsAge >= ctsLifeTime);
	}

	return FALSE;
}
