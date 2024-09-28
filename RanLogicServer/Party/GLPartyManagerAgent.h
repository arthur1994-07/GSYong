#ifndef _GL_PARTY_MAN_H_
#define _GL_PARTY_MAN_H_

#pragma once

#include <deque>
#include <boost/tr1/unordered_map.hpp>
#include <boost/pool/object_pool.hpp>
#include "../Database/DBAction/DbAction.h"
#include "GLPartyAgent.h"

struct NET_MSG_GENERIC;
class GLAgentServer;
class GLPartyManagerAgent
{
public:
	const bool SwapSlot(const GLPartyID& PartyID, const GLSwapSlotInformation& SwapSlotFrom, const GLSwapSlotInformation& SwapSlotTo);
	const bool ConfrontFB( NET_MSG_GENERIC* nmg );

	GLExpeditionAgent*	NewExpedition(const GLPartyID& PartyID, const DWORD dwGaeaID);
	GLPartyAgent* NewParty(const DWORD dwGaeaID );

	const DWORD AddPartyMember(const GLPartyID& PartyID, const DWORD dwGaeaID);

	const bool DeleteParty(const GLPartyID& PartyID);
	const bool DeleteParty(void);
	const bool DeleteExpedition(const GLPartyID& PartyID);	
	const bool removePartyMember(const GLPartyID& PartyID, const DWORD dwGaeaID, BYTE eType = 0);
	const bool DeleteConfrontMember(const GLPartyID& PartyID, const DWORD dwGaeaID);
	const bool removePreMember(const GLPartyID& PartyID, const DWORD dwGaeaID);

	const EMPARTY_ERROR_TYPE GetPartyMemberGaeaIDs(const GLPartyID& PartyID, GLPartyMemberGaeaIDs& MemberGaeaIDs);	
	const GLExpeditionAgent* const GetConstExpedition(const GLPartyID& PartyID) const;
	const GLPartyAgent* const GetConstParty(const GLPartyID& PartyID) const;
	GLExpeditionAgent* const GetExpedition(const GLPartyID& PartyID) const;
	GLPartyAgent* const GetParty(const GLPartyID& PartyID) const;

	const bool SendErrorMsg(const GLPartyID& PartyID, const DWORD dwGaeaID, const EMPARTY_ERROR_TYPE emErrorType, const char* cErrorMsg = NULL) const;
	const bool SendMsgToMember( const GLPartyID& PartyID, NET_MSG_GENERIC* nmg );
    const bool SendMsgToMember( const GLPartyID& PartyID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false );
	const bool SendMsgToMember( DWORD dwGaeaID, const GLPartyID& PartyID, NET_MSG_GENERIC* nmg );	

	HRESULT FrameMove(const float fTime, const float fElapsedTime);
	HRESULT FrameMoveValidExpedition(const float fTime, const float fElapsedTime, GLExpeditionAgent* const pExpedition);
	HRESULT FrameMoveValidParty(const float fTime, const float fElapsedTime, GLPartyAgent* const pParty);

	void AddGameDbJob(db::DbActionPtr spJob);
	void AddGameAdoJob(db::DbActionPtr spJob);

private:
	const bool _refreshExpeditionMaster(const GLPartyID& PartyID);
	const EMPARTY_ERROR_TYPE _refreshParty(const GLPartyID& PartyID, const DWORD* const pGaeaIDs = NULL, const DWORD nGaeaIDs = 0);


public:
	const GLPartyManagerAgent& operator =(const GLPartyManagerAgent&){}
	GLPartyManagerAgent(GLAgentServer* const pServer, DWORD dwMaxClient);
	~GLPartyManagerAgent();

private:
	typedef std::deque<DWORD>								DEQUE_FREEID;
	typedef std::map<DWORD, GLPartyAgent*>			PARTY_MAP;
	typedef std::map<DWORD, GLExpeditionAgent*>	EXPEDITION_MAP;
	typedef PARTY_MAP::iterator           		PARTY_MAP_ITER;
	typedef PARTY_MAP::const_iterator     	PARTY_MAP_CITER;
	typedef PARTY_MAP::value_type         	PARTY_MAP_VALUE;
	typedef EXPEDITION_MAP::iterator         	EXPEDITION_MAP_ITER;
	typedef EXPEDITION_MAP::const_iterator	EXPEDITION_MAP_CITER;
	typedef EXPEDITION_MAP::value_type    	EXPEDITION_MAP_VALUE;

	GLAgentServer* const m_pAgentServer;
	const WORD m_nMaxClient;

	PARTY_MAP			m_Party;
	EXPEDITION_MAP	m_Expedition;

	boost::object_pool<GLPartyAgent> m_PartyPool;
	boost::object_pool<GLExpeditionAgent> m_ExpeditionPool;
	
	DEQUE_FREEID		m_FreePartyID;
	DEQUE_FREEID		m_FreeExpeditionID;
};

#endif // _GL_PARTY_MAN_H_
