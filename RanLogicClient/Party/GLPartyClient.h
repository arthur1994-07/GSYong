#ifndef _GL_PARTY_CLIENT_H_
#define _GL_PARTY_CLIENT_H_

#pragma once

#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Party/GLPartyDefine.h"

#include "../Widget/GLWidgetScript.h"

struct NET_MSG_GENERIC;
class GLGaeaClient;

struct GLPartySlotClient
{
	DWORD				m_dwGaeaID;
	DWORD				m_dwDbNum;
	EMCHARCLASS		m_emClass;
	MapID				m_sMapID;

	D3DXVECTOR3	m_vPosition;
	GLDWDATA		m_sHP;
	
	int				m_Level;	
	WORD		m_wSchool;
	
	DWORD m_dwPartyMemberState; // EMPARTY_MEMBER_STATE_TYPE;

	char			m_szName[CHAR_SZNAME];	
	

	const GLPartySlotClient& operator =(const GLPartySlotClient& rhs);	
	const GLPartySlotClient& operator =(const GLPARTY_FNET& rhs);
	GLPartySlotClient(void);
	GLPartySlotClient(const GLPARTY_FNET& rhs);
	GLPartySlotClient(const GLPartySlotClient& rhs);
	void Reset(void);

	const bool isValid(void) const;
	const bool isValidMap(void) const;
	const bool isOneSelf(GLGaeaClient* const pGaeaClient) const;
	const bool isOneSelf(const DWORD dwGaeaID) const;
	const bool isMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState) const;

    void SetName(const char* const szName);

	void SetMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState);
	void ResetMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState);
	void InverseMemberState(const EMPARTY_MEMBER_STATE_TYPE emPartyMemberState);
};

class GLPartyClient
{
public:
    GLPartyClient(GLGaeaClient* pGaeaClient, const DWORD dwPartyIndex = MASTER_PARTY);
    virtual ~GLPartyClient(void);

public:
	typedef std::pair<DWORD, GLPartySlotClient> PARTYPAIRCLIENT;
	typedef std::vector<PARTYPAIRCLIENT>	PARTYMAP;
	typedef PARTYMAP::iterator				PARTYMAP_ITER;
	typedef PARTYMAP::const_iterator		PARTYMAP_CITER;

	class FindMemberKey
	{
	public:
		const FindMemberKey& operator =(const FindMemberKey&){}
		const bool operator()(const PARTYPAIRCLIENT& _pair)
		{
			return _pair.first == this->dwGaeaID;
		}
		FindMemberKey(const DWORD _dwGaeaID)
			: dwGaeaID(_dwGaeaID)
		{
		}

		const DWORD dwGaeaID;
	};

    GLGaeaClient* const m_pGaeaClient;
	const DWORD m_nPartyIndex;

	GLPartyID m_PartyID;

	PARTYMAP m_mapParty;
	SPartyOption m_sOption;

	bool m_bQBoxEnable;

public:
	const GLPartyID& GetPartyID(void) const { return m_PartyID; }
	const DWORD GetSimplePartyID() { return m_PartyID.GetPartyID(); }
	const SPartyOption& GetOption(void) const { return m_sOption; }
	const DWORD GetNMember(void) const;
	const DWORD GetMemberIndex(const DWORD dwGaeaID) const;
	const DWORD GetPartyIndex(void) const;    
	const bool IsLevelPartyMember(const WORD wMin, const WORD wMax);
	GLPartySlotClient* const GetMember(const DWORD dwIndex);
	GLPartySlotClient* const GetMemberByGaeaID(const DWORD dwGaeaID);
    GLPartySlotClient* const GetMemberByChaDbNum(const DWORD& dwChaDbNum);
    const char* GetMemberChaName(const DWORD dwIndex);
    const char* GetMemberChaNameByGaeaID(const DWORD dwGaeaID);
    const char* GetMemberChaNameByChaDbNum(const DWORD& dwChaDbNum);

	const bool removeMember(const DWORD dwGaeaID);	


	void SetOption(const SPartyOption& PartyOption) { m_sOption = PartyOption; }
	void SetPartyID(const GLPartyID& PartyID);

	const bool isMaster(void) const;
	const bool isMaster(const DWORD dwGaeaID) const;
	const bool isMember(const DWORD dwGaeaID) const;
	const bool isMember( const char* const szCharName ) const;
	const bool isMemberByDbNum(const DWORD dwDbNum) const;
	const bool isValid(void);

public:
	void ResetParty(void);
	void ResetCheckReady(void);

public:
	void Lure(DWORD dwGaeaID);
	void LureName( const char* szName );
	void Secede(DWORD dwIndex);
	void Dissolve(void);
	void Authority(const DWORD dwIndex);

public :
	void InvitePartyReq(); 	// 초대(가이아 아이디를 Lure에 넣어줘서 인자없이 초대하도록, 단, PtoP메뉴를 통해서만 가능)
	LuaTable MemberToLuaTable( const GLPartySlotClient* const pMember );
	LuaTable GetMemberForWidget( DWORD dwIndex );

	WORD GetItemOption() { return (WORD) m_sOption.GetItemOption(); }
	WORD GetMoneyOption() { return (WORD) m_sOption.GetMoneyOption(); }
	WORD GetItemFilterOption() { return (WORD) m_sOption.GetItemFilter(); }

	void ChangeOption( WORD wItemOp, WORD wMoneyOp, WORD wItemFilter );
	bool IsMyCharMaster() { return isMaster(); }

public:
	BOOL MsgPartyFNew(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyFNewUpdate(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyLureFb(NET_MSG_GENERIC* nmg);
    BOOL MsgPartyLureFbWithCharName(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyAdd(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyDel(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyDissolve(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyAuthority(NET_MSG_GENERIC* nmg);

	BOOL MsgPartyMoveMap(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyMbrDetail(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyMbrDetailParty(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyMbrLevelUp(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyMbrUpdate(NET_MSG_GENERIC* nmg);

	BOOL MsgPartyMbrItemPickUp(NET_MSG_GENERIC* nmg);
    BOOL MsgPartyMbrItemPickUpLink(NET_MSG_GENERIC* nmg);

	BOOL MsgPartyMbrRename(NET_MSG_GENERIC* nmg);
	
	BOOL MsgPartyLinkJoinFB(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyLinkJoinANS(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyLinkLureFB(NET_MSG_GENERIC* nmg);
	BOOL MsgPartyLinkRefuseFB(NET_MSG_GENERIC* nmg);

public:
	HRESULT MsgProcess(NET_MSG_GENERIC* nmg);

public:
	BOOL SetConfront(NET_MSG_GENERIC* nmg);
	void ReSetConfront(void);
	void ResetConfrontMember ( DWORD dwGaeaID );
	const DWORD GetNConfrontMember(void) const;
};

typedef std::tr1::shared_ptr<GLPartyClient> GLPartyClientPtr;

class GLExpeditionClient : public GLExpedition
{
public:
	const bool isMasterChatting(void) const;
	void SetMasterChatting(const bool bMasterChatting);

public:
	GLExpeditionClient(void);
	virtual ~GLExpeditionClient(void);

private:
	bool m_bMasterChatting;
};

#endif // _GL_PARTY_CLIENT_H_