#ifndef _GL_PARTY_FIELD_MAN_H_
#define _GL_PARTY_FIELD_MAN_H_

#pragma once

//#include <map>
#include <boost/tr1/unordered_map.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/noncopyable.hpp>
#include "../../SigmaCore/Memory/ObjectPool.h"

#include "./GLPartyField.h"

struct NET_MSG_GENERIC;
class GLGaeaServer;
class GLPartyID;
class GLExpedition;

class GLPartyManagerField : private boost::noncopyable
{
public:
    GLPartyManagerField(GLGaeaServer* pGaeaServer, DWORD dwMaxClient);
    ~GLPartyManagerField(void);

protected:
    //typedef std::map<DWORD, GLPARTY_FIELD*>  PARTY_MAN;
    typedef std::tr1::unordered_map<DWORD, GLPartyField*>  PARTY_MAN; //! PartyID <-> GLPARTY_FIELD
    typedef PARTY_MAN::iterator						PARTY_MAN_ITER;	
    typedef PARTY_MAN::const_iterator			PARTY_MAN_CITER;
    typedef PARTY_MAN::value_type				PARTY_MAN_VALUE;
	typedef std::map<DWORD, GLExpedition*>	EXPEDITION_MAP;
	typedef EXPEDITION_MAP::iterator         		EXPEDITION_MAP_ITER;
	typedef EXPEDITION_MAP::const_iterator		EXPEDITION_MAP_CITER;
	typedef EXPEDITION_MAP::value_type    		EXPEDITION_MAP_VALUE;

    sc::BOOST_OBJECT_POOL_LOCK_CHECKER<GLPartyField>	m_PoolParty; //! GLPARTY_FILED �޸� ����
	sc::BOOST_OBJECT_POOL_LOCK_CHECKER<GLExpedition>	m_ExpeditionPool;
    PARTY_MAN m_PartyMan; //! GLPARTY_FIELD ����
	EXPEDITION_MAP	m_Expedition;; //! ������ ����;
    //GLPARTY_FIELD*	m_pParty;	

    GLGaeaServer* m_pGaeaServer;
	DWORD m_dwMaxClient;

    typedef std::tr1::unordered_map<DWORD, DWORD> GAEA_PARTY_MAN; //! GaeaId <-> PartyID
    typedef GAEA_PARTY_MAN::iterator           GAEA_PARTY_MAN_ITER;
    typedef GAEA_PARTY_MAN::const_iterator     GAEA_PARTY_MAN_CITER;
    typedef GAEA_PARTY_MAN::value_type         GAEA_PARTY_MAN_VALUE;
    GAEA_PARTY_MAN m_GaeaParty; //! GaeaId <-> PartyID    

public:
	GLPartyField* NewParty(const DWORD dwMaster, const GLPartyID& PartyID );
	void DelParty(const GLPartyID& PartyID );
	void DelPartyAll ();	

public:
	GLPartyField* const GetParty(const DWORD dwPartyID) const;
	GLPartyField* const GetParty(const GLPartyID& PartyID) const;
	GLExpedition* const GetExpedition(const DWORD dwExpeditionID) const;
	GLExpedition* const GetExpedition(const GLPartyID& PartyID) const;
	const GLPARTY_FNET* const GetMasterSlot(const GLPartyID& PartyID) const;
	const DWORD GetNMember(const GLPartyID& PartyID) const;	
	void SetPartyID(DWORD dwGaeaID, const GLPartyID& PartyID);
	void resetGaeaIDMap(const DWORD dwGaeaID);

	BOOL IsPartyMaster ( GLPartyID& PartyID, DWORD dwGaeaID );

	const bool SwapSlot(const GLPartyID& PartyID, const GLSwapSlotInformation& SwapSlotFrom, const GLSwapSlotInformation& SwapSlotTo);


public:
	// �ʵ� �������� ��� ��Ƽ���鿡�� �޼����� ������ ���ؼ�;
	// ������Ʈ ������ ���� �ؾ� �ϴµ�, ���� ���ϰ� ŭ;
	// ������Ʈ ��Ȳ�� ȣ�� �ɶ������� ���� �ʴ� ���� ����;

	// �ʵ� ������ ���� ��Ƽ�� ���ο��� ����;
	const bool SendMsgToMember(const GLPartyID& PartyID, NET_MSG_GENERIC* nmg);
	const bool SendMsgToMember(const GLPartyID& PartyID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc = false);

	// ���� �ʿ� ���� �ϴ� ��Ƽ���鿡�� �޼��� �۽�;	
	const bool SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, NET_MSG_GENERIC* nmg);
	const bool SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc = false);

	// ���� �ʿ� �����ϰ� ������ ��ġ �ݰ� ���� �ִ� ��Ƽ���鿡�� �޼��� �۽�;
	const bool SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, NET_MSG_GENERIC* nmg);
	const bool SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc = false);
    
public:
	BOOL MsgProcess(NET_MSG_GENERIC* nmg, const DWORD dwGaeaID);

private:	
	BOOL MsgPartyFNew(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyFNewUpdate(NET_MSG_GENERIC* nmg);

	BOOL MsgPartyAdd(NET_MSG_GENERIC* nmg);	
	BOOL MsgPartyDel(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyDissolve(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyAuthority(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyMbrMoveMap(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyMbrRename(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyChangeOption(NET_MSG_GENERIC* nmg);	
	BOOL MsgExpeditionFNew(NET_MSG_GENERIC* nmg);	
	BOOL MsgExpeditionSwapSlot(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyRefreshRequestCF(NET_MSG_GENERIC* nmg, const DWORD dwGaeaID);

private:	
	const bool _refreshExpeditionMaster(const GLPartyID& PartyID);
	BOOL isValidPartyID(const GLPartyID& PartyID, const DWORD dwGaeaID);
	BOOL SendMsgToMember_PartyDetail(const GLPartyID& PartyID, const DWORD dwGaeaID);
	BOOL Dissolve(const GLPartyID& PartyID);
	
};

#endif // _GL_PARTY_FIELD_MAN_H_