#pragma once

#include "GLPartyClient.h"

struct NET_MSG_GENERIC;
class GLPartyManagerClient
{
public:
	const bool isParty(void);
	const bool isMasterChatting(void) const; // ������ ä�� ����� ���� �ִ°�;
	const bool isExpedition(void); // ������ �����ΰ�;
	const bool isPartyMaster(void) const; // ��Ƽ���ΰ�;
	const bool isExpeditionMaster(void) const; // ���������ΰ�;
	const bool isMaster(void) const; // ��Ƽ�� Ȥ�� ������ ���ΰ�;
	const bool isExpeditionSubMaster(void) const; // ������ �����ΰ�;

public: // ����� ���� ����Ÿ ��û;
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
	void Lure(const DWORD dwGaeaID); // �ʴ�;	
	void LureByDbNum(const DWORD dwDbNum); // �ʴ�;
	void Secede(const DWORD dwGaeaID, BYTE eType = 0); // �߹�/Ż��;
	void Dissolve(void); // ��Ƽ ��ü;
	void Authority(const DWORD dwNewMasterGaeaID, const DWORD dwPartySlot = 0, const bool bExpedition = false); // ��Ƽ/�������� ����;
	void CheckReadyRequest(void); // �غ� Ȯ��;
	void CheckReadyAnswer(const bool bReady); // �غ� Ȯ�� ����;
	void SwapSlot(const GLSwapSlotInformation& SwapSlotFrom, const GLSwapSlotInformation& SwapSlotTo); // �ڸ� �̵�;
	void Expedition(void); // ������ ����;
	void MasterChatting(void); // ������ ä��;
	void ChangeOption(const SPartyOption& PartyOption); // ��Ƽ �ɼ� ����;
	void Nominate(const GLSwapSlotInformation& _NewSubMasterInfo); // ������ ���� �Ӹ�;
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
	// ��Ƽ �ʴ� ����/���� ����
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
	GLPartyID m_PartyID; // �÷��̾ ���� ��Ƽ/������ ID;

	DWORD m_dwRequestGaeaID; // ��Ƽ �ʴ����� GaeaID; �ӽ� ����;		
};


