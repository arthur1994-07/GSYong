#pragma once

#include <map>
#include <vector>

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Item/GLItemCustom.h"

enum
{
	PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE = 5
	// ������ ����/������ �й� UI�� ǥ��� �������� �ִ� ����;
	// ��UI�� ���������� �ʱ� ������ �� ���� ���� �Ѵٰ� �ؼ� ���� �� ���ư��� ����;
	// ������ �ʿ��ϰ� �� ��� �ݵ�� �� ������ �˻� �� ���� UI�� ũ�� �� ǥ�� ������ ���� �� �־�� ��;
};
struct GLTenderItemCustomClient : public GLTenderItemCustom
{
public:
	GLTenderItemCustomClient(const MapID& mapID, const SITEMCUSTOM& ItemCustom, const float fInsertTime, const D3DXVECTOR3& vGeneratePosition);

	const SITEMCUSTOM m_ItemCustom;
	EMPARTY_TENDER_STATE_TYPE m_emTenderState;
	bool m_bSelected;
	bool m_bShow;
	bool m_bNew;
	float m_fAccumTime;
	float m_fPercent;
};

class GLGaeaClient;
struct NET_MSG_GENERIC;
class GLPartyTenderManagerClient
{ // �ֻ��� ���� ������ �޴���;
	typedef std::map<DWORD, GLTenderItemCustomClient>	ItemCustomMap;
	typedef ItemCustomMap::iterator								ItemCustomMapIter;
	typedef ItemCustomMap::const_iterator						ItemCustomMapCIter;	
	typedef ItemCustomMap::value_type							ItemCustomMapValue;

	typedef std::vector<DWORD>						ItemCustomVector;
	typedef ItemCustomVector::iterator			ItemCustomVectorIter;
	typedef ItemCustomVector::const_iterator	ItemCustomVectorCIter;	
public:
	void Reset(void);

public: // Call by UI;
	const DWORD GetNItem(void) const;
	const DWORD GetItemID(const DWORD dwIndex) const;
	GLTenderItemCustomClient* const GetTenderItemCustom(const DWORD dwIndex, const bool bDistribution = false);
	GLTenderItemCustomClient* const GetDistributionItemCustom(const DWORD dwIndex);
	const GLTenderItemCustomClient* const GetSelectedItemCustom(void) const;
	
	// SF
	LuaTable GetTenderItem(void);
	LuaTable GetMemberInfo(void);
	void SetSelectMember(DWORD dwSelectSlot);

	const bool isTendering(void) const;
	const bool isNeedRefresh(void) const;
	const bool isExistNoneAvailableItem(void) const;
	const bool isRangeTenderItem(const DWORD dwItemID);	// IDŰ�� �������� �й���� �� �ִ� �Ÿ����� �Ǻ�;
	void RefreshTenderItemIsNew(void);	// ������ �й�â ���� �� ���� ������ �������� �ƴ϶�� ����;
	void DoneRefresh(void);
	

	const bool SetSelect(const DWORD dwSelectSlotIndex);
	void SetBeginIndex(const DWORD dwBeginIndex);
	void ResetSelectByItemID(const DWORD dwItemID);
	
	void TenderItem(void) const; // ������ ����(�ֻ���);
	void AbandonItem(void); // ���� �� ����(�ֻ���);
	void TransferItem(void); // ������ �絵(������ �й�);
	void JunkItem(void); // ������ ������(������ �й�);
	void ResetSelectAll(void);	// ��� ������ ���� ��� 

public: // Call by Main FrameMove;
	const HRESULT FrameMove(const float fTime, const float fElapsedTime);
		void FrameMove_LeaderShare(const GLPartyID& PartyID);
		void FrameMove_ItemDice(void);

public: // Call by Network Message procedure;
	const bool MsgProcess(NET_MSG_GENERIC* pMessage);

private:
	const bool MsgPartyAddTenderItem(NET_MSG_GENERIC* pMessage);
	const bool MsgPartyTenderItem(NET_MSG_GENERIC* pMessage);
	const bool MsgPartyTenderItemFb(NET_MSG_GENERIC* pMessage);
	const bool MsgPartyJunkItem(NET_MSG_GENERIC* pMessage);

public:
	GLPartyTenderManagerClient(GLGaeaClient* const pGaeaClient);
	virtual ~GLPartyTenderManagerClient(void);

private:
	GLGaeaClient* const m_pGaeaClient;

	// ItemCustom ����ü�� ũ�Ⱑ ���� ���� �ƴϹǷ�;
	// ���� ����Ÿ�� ������ID�� Ű�� ������ ������ ����;	
	ItemCustomMap m_ItemCustomMap;
	// ����� ������ ������� ���� �ϱ� ���� ������ID�� ���� ���Ϳ� �־ ����;
	ItemCustomVector m_ItemCustomVector;
	
	// �� �÷��װ� true �ΰ�쿡�� �ֻ���/������ �й� UI ���� ��;
	// ������ ����Ʈ�� ��ȭ�� ���� ��� �� �÷��׸� true�� ���� �ٲ���;
	bool m_bNeedRefresh; 

	DWORD m_dwBeginIndex;

	// ������ �й� UI���� ���� �� �ִ� ������ID�� ����;
	// ������ �й� UI�� �ߺ� ������ ������ �ֻ��� UI�ʹ� �޸� ���� ���ø� ������;
	DWORD m_dwSelectedItem;
};

