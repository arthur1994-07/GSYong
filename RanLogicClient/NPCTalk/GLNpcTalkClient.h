#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

#include "../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"
#include "../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../RanLogic/RANPARAM.h"

#include "../Widget/GLWidgetScript.h"

// 카트 저장용
struct SNpcMarketCart
{
	DWORD dwIndex;
	SNATIVEID sID;
	WORD wPosX;
	WORD wPosY;
	WORD wCount;

	SNpcMarketCart()
		: dwIndex( 0 )
		, sID( NATIVEID_NULL() )
		, wPosX( 0 )
		, wPosY( 0 )
		, wCount( 0 )
	{
	}

	SNpcMarketCart( DWORD _dwIndex, const SNATIVEID& _sID, WORD _wPosX, WORD _wPosY, WORD _wCount )
		: dwIndex( _dwIndex )
		, sID( _sID )
		, wPosX( _wPosX )
		, wPosY( _wPosY )
		, wCount( _wCount )
	{
	}
};
typedef std::vector< SNpcMarketCart >	VEC_NPCMARKET_CART;
typedef VEC_NPCMARKET_CART::iterator	VEC_NPCMARKET_CART_ITER;

class GLNpcTalkClient
{
private :
	GLNpcTalkClient(void);
	GLNpcTalkClient( const GLNpcTalkClient& value );
	~GLNpcTalkClient(void);

public :
	static GLNpcTalkClient& GetInstance();

private :
	static const int nSTARTINDEX;

	// 답변 타입 - 수정시 WidgetDef.lua 파일 같이 수정
	enum PRIORITY_TALK_ORDER
	{
		TALK_FIRST_TIME = 1,
		TALK_QUEST_STEP = 2,
		TALK_QUEST_START = 3,
		TALK_QUEST_ING = 4,
		TALK_BASIC_TALK = 5,
	};

private :
	SNATIVEID m_sNpcID;			// 현재 대화 시도한 NPC ID
	DWORD	  m_dwNpcGlobalID;		// 현재 대화 시도한 NPC 글로벌 ID
	EMCROW	  m_emCrow;

	std::tr1::shared_ptr< CNpcDialogue > m_spDlg;
	std::tr1::shared_ptr< CNpcDialogueSet > m_spDlgSet;
	std::tr1::shared_ptr< CNpcDialogueCase > m_spDlgCase;

	std::vector< std::tr1::shared_ptr< SNpcTalk > > m_vecTalk;
	std::multimap< int, int > m_mapTalkIndex;

	VEC_NPCMARKET_CART m_vecMarketCart;		// 카트 리스트

	DWORD m_dwRandomTime;
	int m_nRebuyTabIndex;
	bool m_bEnabledMarket;

	LONGLONG m_llTotalCartPrice;

public :
	// 대화 데이터 설정
	void SetTalkData( const SNATIVEID& sNpcID, DWORD dwNpcGlobalID, EMCROW emCrow = CROW_NPC );
	// 대화문 선택
	bool SelectTalk( DWORD dwAnswerID );

	// 판매 중인 아이템 리스트
	LuaTable GetSaleItem( DWORD dwIndex );
	// 추천하는 아이템 리스트
	LuaTable GetRcommendSaleItem( WORD wMaxCount, DWORD dwIndex );

	// NPC 대사
	std::string GetBasicTalk();
	// 답변 리스트
	LuaTable GetAnswerList();
	// 마켓탭 정보
	LuaTable GetMarketTab();

	// 추천 아이템 - 사용 가능 아이템만 추천?
	void SetUsableItem( bool bValue ) { RANPARAM::bCheckUsableItemMarketPage = bValue; }
	bool GetUsableItem() { return RANPARAM::bCheckUsableItemMarketPage; }

	// 추천 아이템 - 구입 가능 아이템만 추천?
	void SetPurchaseItem( bool bValue ) { RANPARAM::bCheckBuyableItemMarketPage = bValue; }
	bool GetPurchaseItem() { return RANPARAM::bCheckBuyableItemMarketPage; }

	// 오른쪽 마우스 클릭 아이템 판매 설정
	void SetSellRMouse( bool bValue ) { RANPARAM::bCheckSellItemRButton = bValue; }
	bool GetSellRMouse() { return RANPARAM::bCheckSellItemRButton; }

	// 아이템 판매시 확인 설정
	void SetConfirmSellItem( bool bValue ) { RANPARAM::bCheckConfirmSellItem = bValue; }
	bool GetConfirmSellItem() { return RANPARAM::bCheckConfirmSellItem; }

	// 소모품 판매시 재매입에 등록할 것인지 설정
	void SetRebuySupplies( bool bValue );
	bool GetRebuySupplies() { return RANPARAM::bCheckRebuySuppliesRebuyPage; }

	// 재매입 아이템이 있을 경우 접속 종료시 확인 설정
	void SetRebuyItemDeleteConfirm( bool bValue ) { RANPARAM::bCheckAllDelItemsRebuyPage = bValue; }
	bool GetRebuyItemDeleteConfirm() { return RANPARAM::bCheckAllDelItemsRebuyPage; }

	// 아이템 구매 (활성 페이지, 아이템 슬롯 위치)
	void BuyItem( DWORD dwPageIndex, WORD wPosX, WORD wPosY, WORD wBuyNum );
	// 아이템 판매 (인벤 위치)
	void SellItem( WORD wPosX, WORD wPosY );

	// 카트에 아이템 넣기
	void AddCart( DWORD dwPageIndex, WORD wMID, WORD wSID, WORD wPosX, WORD wPosY, WORD wCount );
	// 카트에 아이템 빼기
	void RemoveCart( DWORD dwIndex );

	// 카트 전부 비우기
	void RemoveCartAll();
	// 카트에 넣은 아이템 구매
	bool BuyCartItem(WORD wTabPage);

	// 재매입
	bool Rebuy( WORD wIndex );

	// 카트 리스트
	LuaTable GetCart();
	// 카트에 올라간 아이템 가격
	LONGLONG GetCartTotalPrice() { return m_llTotalCartPrice; }
	// 재매입 아이템 리스트
	LuaTable GetRebuy();
	// 재매입 아이템 개수
	WORD GetRebuyCount();
	// 광고 아이템 ID
	DWORD GetAdItemID();
	// 광고 아이템 검색어
	std::string GetAdItemSearchString();
	// NPC 이용가능 체크;
	const bool IsUseable();
	const bool IsUseableFromID( DWORD dwNpcID );

	const EMCROW GetTalkCrow() { return m_emCrow; }

private :
	void LoadNode( DWORD dwID, bool bRandom = false, DWORD dwRandomTime = 0 );
	void LoadTalkList( bool bRandom );

	bool CheckQuestProc( std::tr1::shared_ptr< SNpcTalk > spNpcTalk, DWORD dwPartyMemberNum );
	bool SelectBasic( std::tr1::shared_ptr< SNpcTalk > spNpcTalk );
	void SelectQuestStart( std::tr1::shared_ptr< SNpcTalk > spNpcTalk );
	void SelectQuestStep( std::tr1::shared_ptr< SNpcTalk > spNpcTalk );

	// 추천 아이템 조건 검사
	bool CheckRecommendItem( const SITEM* pItemData, const SITEMCUSTOM& sItemCustom,
		bool bTypeCheck, DWORD dwIndex, WORD wPosX, WORD wPosY );
	// 교환 조건 검사
	bool CheckExchange( const SITEM* pItemData, DWORD dwIndex, WORD wPosX, WORD wPosY );

	// 카트 아이템 가격 계산
	void CalcCartItemPrice();

public :
	void SetNpcID( const SNATIVEID& sID ) { m_sNpcID = sID; }
	const SNATIVEID& GetNpcID() { return m_sNpcID; }

	void SetNpcGlobalID( DWORD dwID ) { m_dwNpcGlobalID = dwID; }
	DWORD GetNpcGlobalID() { return m_dwNpcGlobalID; }
};
