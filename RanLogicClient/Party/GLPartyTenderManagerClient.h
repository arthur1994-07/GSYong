#pragma once

#include <map>
#include <vector>

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Item/GLItemCustom.h"

enum
{
	PARTY_TENDER_WINDOW_ITEM_SLOT_NSIZE = 5
	// 아이템 입찰/마스터 분배 UI에 표기될 아이템의 최대 갯수;
	// 란UI는 가변적이지 않기 때문에 위 값을 수정 한다고 해서 모든게 잘 돌아가지 않음;
	// 수정이 필요하게 될 경우 반드시 위 값으로 검색 후 관련 UI의 크기 및 표시 로직을 수정 해 주어야 함;
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
{ // 주사위 입찰 아이템 메니저;
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
	const bool isRangeTenderItem(const DWORD dwItemID);	// ID키의 아이템이 분배받을 수 있는 거리인지 판별;
	void RefreshTenderItemIsNew(void);	// 아이템 분배창 오픈 시 새로 떨어진 아이템이 아니라고 설정;
	void DoneRefresh(void);
	

	const bool SetSelect(const DWORD dwSelectSlotIndex);
	void SetBeginIndex(const DWORD dwBeginIndex);
	void ResetSelectByItemID(const DWORD dwItemID);
	
	void TenderItem(void) const; // 아이템 입찰(주사위);
	void AbandonItem(void); // 입찰 외 포기(주사위);
	void TransferItem(void); // 아이템 양도(마스터 분배);
	void JunkItem(void); // 아이템 버리기(마스터 분배);
	void ResetSelectAll(void);	// 모든 아이템 선택 취소 

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

	// ItemCustom 구조체의 크기가 작은 편이 아니므로;
	// 실제 데이타는 아이템ID를 키로 가지는 맵으로 구성;	
	ItemCustomMap m_ItemCustomMap;
	// 드랖된 아이템 순서대로 정렬 하기 위해 아이템ID만 따로 벡터에 넣어서 관리;
	ItemCustomVector m_ItemCustomVector;
	
	// 이 플래그가 true 인경우에만 주사위/마스터 분배 UI 갱신 함;
	// 아이템 리스트에 변화가 생긴 경우 이 플래그를 true로 직접 바꿔줌;
	bool m_bNeedRefresh; 

	DWORD m_dwBeginIndex;

	// 마스터 분배 UI에서 선택 되 있는 아이템ID를 저장;
	// 마스터 분배 UI는 중복 선택이 가능한 주사위 UI와는 달리 단일 선택만 가능함;
	DWORD m_dwSelectedItem;
};

