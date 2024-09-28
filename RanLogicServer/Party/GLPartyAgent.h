#pragma once

#include <hash_set>
#include <hash_map>
#include <vector>

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxLand/DxLandDef.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Party/GLPartyDefine.h"
#include "GLPartyNotifier.h"

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include <strsafe.h> // Safe string function

class GLAgentServer;

struct GLPartyAgent
    : public GLPartyNotifier<GLPartyAgent>
{
public:
	typedef std::vector<DWORD> MEMBER;
	typedef MEMBER::iterator MEMBER_ITER;
	typedef MEMBER::const_iterator	MEMBER_CITER;

	GLPartyID	m_PartyID; //! 파티 고유번호;

	MEMBER m_Member; // 파티에 참가한 현재 인원의 GaeaID;
	MEMBER m_PreMember; // 예비 맴버;
	SPartyOption m_PartyOption; // 파티 옵션;
	

	float m_fConfrontTimer; // 대련 타이머;
	MEMBER m_StartConfrontMember; // 대련 초기 참가자;
	MEMBER m_CurrentConfrontMember; // 대련 현제 참가자. ( 초기 인원에서 '멥이동/패배'등으로 감소함 );

	SCONFT_OPTION	m_ConfrontOption; // 대련 옵션;
	GLPartyID m_ConfrontTargetPartyID; // '파티대련' 의 상대 파티;

public:
	GLPartyAgent(void);	
	void reset(void);

    const SPartyOption& GetPartyOption() const { return m_PartyOption; }
    const GLPartyID& GetPartyID(void) const; //! 파티 고유번호;
	const MEMBER& GetMember(void) const; //! 파티에 참가한 현재 인원. GaeaID;

	const DWORD getMaster(void) const; // 파티장의 GaeaID;
	const DWORD changeMaster(GLAgentServer* const pServer); // 다음 슬롯의 파티원에게 파티장 위임, 실패시 ret : GAEAID_NULL;
	const bool changeMaster(GLAgentServer* const pServer, const DWORD dwNewMasterGaeaID);
	void setMaster(const GLPartyID& PartyID, const DWORD dwGaeaID);

	const bool isValidParty(void) const;
	const bool isMaster(const DWORD dwGaeaID) const;
	const bool isFull(void) const;

	// party;
	const bool isPartyMember(const DWORD dwGaeaID) const;

	const DWORD getNPartyMember(void) const;	
    const bool IsLevelPartyMember(GLAgentServer* const pServer, const WORD wMin, const WORD wMax);
	const DWORD addPartyMember(GLAgentServer* const pServer, const DWORD dwGaeaID);
	const bool removePartyMember(GLAgentServer* const pServer, const DWORD dwGaeaID);

	// pre-Member;
	const bool isPreMember(const DWORD dwGaeaID);

	const DWORD addPreMember(const DWORD dwGaeaID);
	const bool removePreMember(const DWORD dwGaeaID);
	

	// confront;
	const bool isConfronting(void) const;
	const bool isLoseConfront(void) const;

	const DWORD getNConfrontMember(void) const;

	void addConfrontMember(const DWORD dwGaeaID);
	void removeConfrontMember(const DWORD dwGaeaID);
	void refreshConfrontMember(void); // 현제 m_conftCURRENT의 값을 m_conftSTART 에 대입;
	void resetConfront(GLAgentServer* pServer);	
	


	bool IsSAME_SCHOOL(GLAgentServer* pServer);
	WORD GetMASTER_SCHOOL(GLAgentServer* pServer);
	const MapID GetMASTER_MAP(GLAgentServer* pServer);
};

class GLExpeditionAgent : public GLExpedition
{
public:
	const bool isPreMember(const DWORD dwGaeaID) const;

	const bool addPreMember(const DWORD dwGaeaID);
	const bool deletePreMember(const DWORD dwGaeaID);

public:
	GLExpeditionAgent(void);
	virtual ~GLExpeditionAgent(void);
public:
	typedef std::vector<DWORD>		MEMBER;
	typedef MEMBER::iterator			MEMBER_ITER;
	typedef MEMBER::const_iterator	MEMBER_CITER;

	MEMBER		m_PreMember;		//	예비 맴버.
};

/*
//	Note : 파티 장만 남은 파티의 경우 자동 소멸;
//
if ( (pParty->getNPartyMember() < 2) && (bExpedition == false) )
{
if ( pParty->isConfronting() == true )
{
GLPartyAgent* pConftParty = GetParty(pParty->m_ConfrontTargetPartyID);
if ( pConftParty != NULL )
pConftParty->resetConfront(m_pAgentServer);
pParty->resetConfront(m_pAgentServer);

GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld(PartyID, pParty->m_ConfrontTargetPartyID, EMCONFRONT_END_FAIL, EMCONFRONT_END_FAIL);
m_pAgentServer->SENDTOALLCHANNEL(&NetMsgFld);
}

DeleteParty(PartyID);

#ifndef NDEBUG
CString strErrorMsg;
strErrorMsg.Format(
"[Party] FrameMove E/PartyID : %d/%d - NMember under 2", 
PartyID.GetExpeditionID(), PartyID.GetPartyID());
sc::writeLogError(strErrorMsg.GetString());
#endif
}
*/
