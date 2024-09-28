#ifndef _GL_PARTY_FIELD_H_
#define _GL_PARTY_FIELD_H_

#pragma once

#include "../../EngineLib/DxLand/DxLandDef.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Party/GLPartyDefine.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

class CItemDrop;
class CMoneyDrop;
class GLChar;
class GLLandMan;
class GLGaeaServer;

typedef DWORD PartyMemberGaeaIDs[MAXPARTY];

struct GLPartyField
{
public:
    GLPartyField(GLGaeaServer* pGaeaServer);

private:
    GLGaeaServer* m_pGLGaeaServer;

public:	
	void RESET();	

	const DWORD GetNMember(void) const;
	const DWORD GetMasterGaeaID(void) const;	
	const DWORD GetPartyMemberGaeaIDs(PartyMemberGaeaIDs& dwPartyMemberGaeaIDs) const;
	const GLPartyID& GetConfrontPartyID(void) const;
	const GLPartyID& GetPartyID(void) const;
	const SPartyOption& GetPartyOption(void) const;
	const DWORD GetMemberSlotIndex(const DWORD dwGaeaID) const;
	GLPARTY_FNET* const GetMemberSlot(const DWORD dwGaeaID) const;	
	const GLPARTY_FNET* const GetMasterSlot(void) const;

	const bool isConfront(const DWORD dwGaeaID) const;
	const bool isMasterByGaeaID(const DWORD dwGaeaID) const;
	const bool isMemberByGaeaID(const DWORD dwGaeaID) const;

	const SCONFT_OPTION& GetConfrontOption(void) const;
	const bool MBR_CONFRONT_JOIN(const DWORD dwGaeaID);
	const bool MBR_CONFRONT_LEAVE(const DWORD dwGaeaID);
	const DWORD GETAVER_CONFT_LEVEL(GLGaeaServer* const pGaeaServer);
	void RECEIVELIVINGPOINT(GLGaeaServer* const pGaeaServer, const int nLivePoint);
	void RESET_CONFRONT(GLGaeaServer* const pGaeaServer, const EMCONFRONT_END emEND);

//	const bool SENDPICKUPMSG2NEAR(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, SITEMCUSTOM* pItemCustom);	

	const bool DOITEM_ALLOT(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, CItemDrop* pItemDrop);
	const bool DOMONEY_ALLOT(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, CMoneyDrop* pMoneyDrop, GLLandMan* pLandMan);	
	
	void SetQBox(const bool bEnable);	
	void SetConfrontOption(const SCONFT_OPTION& ConfrontOption);
	void SetConfrontPartyID(const GLPartyID& PartyID);
	void SetPartyID(const GLPartyID& PartyID);

	void LogMonsterKill(GLGaeaServer* const pGaeaServer, const SNATIVEID& MonsterID, const std::string& MonsterName);	

	void SetMemberSlot(const DWORD dwSlotIndex, const DWORD dwGaeaID, const GLPARTY_FNET& MemberSlot);

	BOOL ISVAILD();	
	BOOL ISFULL();	

	const DWORD ADDMEMBER(GLPARTY_FNET& sParty);
	const bool removeMember(GLGaeaServer* const pGaeaServer, DWORD dwGaeaID);

	BOOL MBR_MOVEMAP(const DWORD dwGaeaID, const MapID& sMapID);
	const MapID GETMBR_MAP(DWORD dwGaeaID);
	
//    bool SENDMSG2NEAR(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, NET_MSG_GENERIC* pNetMsg);    

    // 단순히 모든 인원을 살리는데 이용한다.
    // 일부 혹은 거리 조절이 필요하다면 함수 수정이 필요하다.
    void SetMemberRevival();    

	typedef std::pair<DWORD, GLPARTY_FNET>			PARTYPAIR;

	typedef std::vector<PARTYPAIR>					MEMBER;
	typedef MEMBER::iterator						MEMBER_ITER;
	typedef MEMBER::const_iterator					MEMBER_CITER;

	typedef std::vector<DWORD>						VECROUND;
	typedef VECROUND::iterator						VECROUND_ITER;	

	class FindMember
	{
	public:
		const FindMember& operator =(const FindMember&){}
		const bool operator()(const PARTYPAIR& _pair)
		{
			return _pair.first == this->dwGaeaID;
		}
		FindMember(const DWORD _dwGaeaID)
			: dwGaeaID(_dwGaeaID)
		{
		}

		const DWORD dwGaeaID;
	};
	
	GLPartyID	m_PartyID;		//	파티 ID.
	GLPartyID	m_ConfrontPartyID;
	
	MEMBER		m_cMEMBER;			//	멤버 리스트.

	SPartyOption	m_sOPTION;			//	옵션.	

	VECROUND	m_arrayROUND_ITEM_CUR;			//	아이템루팅 순서.
	VECROUND	m_arrayROUND_ITEM[MAXPARTY];	//	아이템루팅 랜덤 순서.

	VECROUND	m_arrayROUND_MONEY_CUR;			//	Money 루팅 순서.
	VECROUND	m_arrayROUND_MONEY[MAXPARTY];	//	Money 루팅 랜덤 순서.

	SCONFT_OPTION	m_ConfrontOption;
	DWORD			m_dwConfrontMemberAverageLevel;

	bool			m_bQBoxEnable;	
};

#endif // _GL_PARTY_FIELD_H_