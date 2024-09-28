#pragma once

#include "../Widget/GLWidgetScript.h"
#include "../../RanLogic/Item/GLItemCustom.h"

class RebuildCard
{
public:
	static RebuildCard& GetInstance(void);

//UI 기능별 동작되는 란 로직 내부 함수
public:
	void OpenRebuildUI ( EMTYPE_ITEM_REMODEL _emTypeItemRemodel );			// 개조카드 UI 오픈				

	void ReqRebuildMoveItem(const bool bMsg, const WORD wPreInvenItemPosX, const WORD wPreInvenItemPosY );	// 슬롯에 개조 대상 아이템이 등록시 메시지 발생(Scaleform) (Clinet -> Server)
	void ReqRebuildMoveStamp(void);				// 슬롯에 스템프 등록시 메시지 발생 (Scaleform)(Clinet -> Server)

	void ReceiveRebuildMoveItem( WORD wPosX, WORD wPosY );	// 아이템 등록 확인 메시지	(Server -> Clinet)
	void ReceiveRebuildMoveStemp(void);						// 스템프 등록 확인 메시지	(Server -> Clinet)

	void RemoveRebuildItem(void);				// 슬롯에서 아이템 제거시 발생되는 메시지(Scaleform)
	void RemoveRebuildStemp(void);				// 슬롯에서 스템프 제거시 발생되는 메시지(Scaleform)

	bool CheckItemRebuildWindowClose(const bool bUseStamp, const ITEM_RANDOMOP_UTILL::FixOption& fixOption, const EMTYPE_ITEM_REMODEL emTypeItemRemodel );
	void SetItemRebuildSetCheckBox(const SITEMCUSTOM& sItemCustom, const ITEM_RANDOMOP_UTILL::FixOption& fixOption);

//Lua 호출 함수
public:
	const bool InitRebuild(void);				// 개조카드 관련 사용 변수 초기화						

	void ReqSelectCache(void);					// 결과값 선택 & 롤백;
	void ReqChangeRebuildType(bool bKeepOrigin);
	void ReqRebuildItem(void);					// 개조카드 사용;
	void ReqRebuildItem_Fix(void);				// 개조 확정;
	void ReqRebuildItem_Cancel(void);			// 개조 취소;
	void ReqRebuildItem_DirectFix(void);		// 개조 바로 확정;
	void ReqCloseWindow(void);					//개조카드는 종료되는 메시지가 별도 존재
	
	bool SetFixedOption( const unsigned int nIndex );		// 고정할 옵션 입력
	void ResetFixedOption(void);							// 고정된 옵션 해제
	const int GetFixedOption ( const unsigned int _nIndex );
	const int GetNFixOption(void) const;						// 고정된 옵션 개수
	void UpdateFixedOption ( void );							// 고정된 옵션 관련 Update UI;

	//ToolTip 사용
	const int GetOptionMaxSize(void) const;					//랜덤 옵션 최대 갯수
	const int GetRandomOLock(void) const;	//옵션 고정 가능 갯수
	const LONGLONG GetRebuildCost(void) const;				//개조 비용
	const LONGLONG GetRebuildInput(void) const;				//개조 비용
	
	// 랜덤 옵션
	lua_tinker::table GetRandomOpt(void) const;
	// 랜덤 옵션 정보;
	lua_tinker::table GetRebuild_RandomOpt(void) const;

	// 기본 능력치;
	lua_tinker::table GetBasicStats(void) const;

	// 아이템 스킬;
	lua_tinker::table GetItemSkill(void) const;
	
	void SetPreInventoryItem( WORD wPosX, WORD wPosY );	
	void GetPreInventoryItemPos( WORD& wOutPosX, WORD& wOutPosY );	
	void ResetPreInventoryItem(void);

	// 가산옵션은 툴팁 출력시 모든 타입의 값을 다 넘겨서 값존재로 출력 유무 판단을 한다;
	// 따라서 출력시 타입ENUM의 오름차순으로 출력됨으로 데이터에 존재하는 인덱스와 값이 다를 수 있다;
	// 아래함수는 오름차순 정렬 후 선택 인덱스에 맞는 타입 추출하는 함수;
	// 아래 함수를 사용하지않으려면 툴팁먼저 개선하자;
	INT GetAddonOpFixType(INT nIndex);
	const int GetAddonFixTypeRealIndex ( const BYTE _nType );

public:
	inline const WORD GetFixedBasicStatIndex ( void ) const { return m_wFixedBasicStatIndex; }

private:
	//GLCharData.h
	/*BOOL				m_bRebuildOpen;				// 개조창 오픈 유무
	SINVEN_POS			m_sRebuildItem;				// 개조창 등록된 인벤토리 아이템
	SINVEN_POS			m_sStampItem;				// 개조창 등록된 인장 카드;
	SINVEN_POS			m_sPreInventoryItem;		// 개조용 인벤토리에서 들어올린 임시 아이템	
	RandomOpt::FixOption	m_fixOption;			// 인장 카드로 고정 시킨 아이템 옵션;
	LONGLONG			m_i64RebuildCost;			// 개조시 필요한 금액
	LONGLONG			m_i64RebuildInput;			// 개조창 등록한 금액

	bool				m_bTracingUser;				// 현재 추적중인 유저인지 아닌지*/
	
	SINVEN_POS			m_sPreIventoryItem;
	SITEM*					m_pItemStamp;

	WORD				m_wFixedBasicStatIndex;

private:
	RebuildCard(void);
	RebuildCard( const RebuildCard& value );
	//~RebuildCard(void);
};
