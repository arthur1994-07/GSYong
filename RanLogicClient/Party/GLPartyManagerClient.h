#pragma once

#include "GLPartyClient.h"

struct NET_MSG_GENERIC;
class GLPartyManagerClient
{
public:
	const bool isParty(void);
	const bool isMasterChatting(void) const; // 마스터 채팅 기능이 켜져 있는가;
	const bool isExpedition(void); // 원정대 상태인가;
	const bool isPartyMaster(void) const; // 파티장인가;
	const bool isExpeditionMaster(void) const; // 원정대장인가;
	const bool isMaster(void) const; // 파티장 혹은 원정대 장인가;
	const bool isExpeditionSubMaster(void) const; // 원정대 부장인가;

public: // 출력을 위한 데이타 요청;
	GLPartyClient* const GetMyParty(void);
	GLPartyClient* const GetParty(const DWORD dwPartySlotIndex);
	GLPartyClient* const GetParty(const GLPartyID& PartyID);
	const GLPartyID& GetPartyID(void) const;
	const SPartyOption& GetPartyOption(void) const;
	const DWORD GetMasterGaeaID(void) const;
	const DWORD GetNMember(void) const;
	void ResetCheckReady(void);

public:
	void Reset(void);

public: //! Request from Client to Server;
	void Lure(const DWORD dwGaeaID); // 초대;	
	void LureByDbNum(const DWORD dwDbNum); // 초대;
	void Secede(const DWORD dwGaeaID, BYTE eType = 0); // 추방/탈퇴;
	void Dissolve(void); // 파티 해체;
	void Authority(const DWORD dwNewMasterGaeaID, const DWORD dwPartySlot = 0, const bool bExpedition = false); // 파티/원정대장 위임;
	void CheckReadyRequest(void); // 준비 확인;
	void CheckReadyAnswer(const bool bReady); // 준비 확인 응답;
	void SwapSlot(const GLSwapSlotInformation& SwapSlotFrom, const GLSwapSlotInformation& SwapSlotTo); // 자리 이동;
	void Expedition(void); // 원정대 구성;
	void MasterChatting(void); // 마스터 채팅;
	void ChangeOption(const SPartyOption& PartyOption); // 파티 옵션 변경;
	void Nominate(const GLSwapSlotInformation& _NewSubMasterInfo); // 원정대 부장 임명;
	void TakeItem(const DWORD dwGaeaID);

public:
	const bool MsgProcess(NET_MSG_GENERIC* pMessage);

private:	
	const bool MsgPartyLureTarget(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyLureTargetWithCharName(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyFNew(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyLureFb(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyLureFbWithCharName(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyAdd(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyDel(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyDissolve(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyAuthority(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMoveMap(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMbrDetail(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMbrDetailParty(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMbrLevelUp(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMbrUpdate(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMbrItemPickUp(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMbrItemPickUpLink(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyMbrRename(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyLinkJoinFB(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyLinkJoinANS(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyLinkLureFB(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyLinkRefuseFB(NET_MSG_GENERIC* pPartyMessage);	
	const bool MsgPartyChangeOption(NET_MSG_GENERIC* pPartyMessage);	
	const bool MsgExpeditionFNew(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgExpeditionCheckReadyRequest(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgExpeditionCheckReadyAnswer(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgExpeditionSwapSlot(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgPartyError(NET_MSG_GENERIC* pPartyMessage);
	const bool MsgExpeditionError( NET_MSG_GENERIC* pPartyMessage );
	const bool MsgPartyFNewUpdate(NET_MSG_GENERIC* pPartyMessage);

public :
	// 파티 초대 수락/거절 답장
	void ReqPartyInviteAns( bool bAccept, DWORD dwPartyID, DWORD dwHashKey, DWORD dwMasterID, DWORD dwTimeKey );

private:	
	const bool RequestRefreshParty(const GLPartyID& PartyID, const DWORD emPartyRefreshType);
	const bool _refreshExpeditionMaster(void);

private:
	const DWORD GetPartySlotIndex(const GLPartyID& PartyID);
	const bool isMasterAuthority(void);

public:
	GLPartyManagerClient(GLGaeaClient* const pGaeaClient);
	~GLPartyManagerClient(void);

private:
	GLGaeaClient* const m_pGaeaClient;

	GLPartyClientPtr m_ptrPartyClient[MAX_PARTYGROUP];	
	GLExpeditionClient* const m_pExpedition;
	GLPartyID m_PartyID; // 플레이어가 속한 파티/원정대 ID;

	DWORD m_dwRequestGaeaID; // 파티 초대자의 GaeaID; 임시 변수;		
};


