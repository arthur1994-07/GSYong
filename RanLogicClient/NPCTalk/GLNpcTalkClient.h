#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

#include "../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"
#include "../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../RanLogic/RANPARAM.h"

#include "../Widget/GLWidgetScript.h"

// īƮ �����
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

	// �亯 Ÿ�� - ������ WidgetDef.lua ���� ���� ����
	enum PRIORITY_TALK_ORDER
	{
		TALK_FIRST_TIME = 1,
		TALK_QUEST_STEP = 2,
		TALK_QUEST_START = 3,
		TALK_QUEST_ING = 4,
		TALK_BASIC_TALK = 5,
	};

private :
	SNATIVEID m_sNpcID;			// ���� ��ȭ �õ��� NPC ID
	DWORD	  m_dwNpcGlobalID;		// ���� ��ȭ �õ��� NPC �۷ι� ID
	EMCROW	  m_emCrow;

	std::tr1::shared_ptr< CNpcDialogue > m_spDlg;
	std::tr1::shared_ptr< CNpcDialogueSet > m_spDlgSet;
	std::tr1::shared_ptr< CNpcDialogueCase > m_spDlgCase;

	std::vector< std::tr1::shared_ptr< SNpcTalk > > m_vecTalk;
	std::multimap< int, int > m_mapTalkIndex;

	VEC_NPCMARKET_CART m_vecMarketCart;		// īƮ ����Ʈ

	DWORD m_dwRandomTime;
	int m_nRebuyTabIndex;
	bool m_bEnabledMarket;

	LONGLONG m_llTotalCartPrice;

public :
	// ��ȭ ������ ����
	void SetTalkData( const SNATIVEID& sNpcID, DWORD dwNpcGlobalID, EMCROW emCrow = CROW_NPC );
	// ��ȭ�� ����
	bool SelectTalk( DWORD dwAnswerID );

	// �Ǹ� ���� ������ ����Ʈ
	LuaTable GetSaleItem( DWORD dwIndex );
	// ��õ�ϴ� ������ ����Ʈ
	LuaTable GetRcommendSaleItem( WORD wMaxCount, DWORD dwIndex );

	// NPC ���
	std::string GetBasicTalk();
	// �亯 ����Ʈ
	LuaTable GetAnswerList();
	// ������ ����
	LuaTable GetMarketTab();

	// ��õ ������ - ��� ���� �����۸� ��õ?
	void SetUsableItem( bool bValue ) { RANPARAM::bCheckUsableItemMarketPage = bValue; }
	bool GetUsableItem() { return RANPARAM::bCheckUsableItemMarketPage; }

	// ��õ ������ - ���� ���� �����۸� ��õ?
	void SetPurchaseItem( bool bValue ) { RANPARAM::bCheckBuyableItemMarketPage = bValue; }
	bool GetPurchaseItem() { return RANPARAM::bCheckBuyableItemMarketPage; }

	// ������ ���콺 Ŭ�� ������ �Ǹ� ����
	void SetSellRMouse( bool bValue ) { RANPARAM::bCheckSellItemRButton = bValue; }
	bool GetSellRMouse() { return RANPARAM::bCheckSellItemRButton; }

	// ������ �ǸŽ� Ȯ�� ����
	void SetConfirmSellItem( bool bValue ) { RANPARAM::bCheckConfirmSellItem = bValue; }
	bool GetConfirmSellItem() { return RANPARAM::bCheckConfirmSellItem; }

	// �Ҹ�ǰ �ǸŽ� ����Կ� ����� ������ ����
	void SetRebuySupplies( bool bValue );
	bool GetRebuySupplies() { return RANPARAM::bCheckRebuySuppliesRebuyPage; }

	// ����� �������� ���� ��� ���� ����� Ȯ�� ����
	void SetRebuyItemDeleteConfirm( bool bValue ) { RANPARAM::bCheckAllDelItemsRebuyPage = bValue; }
	bool GetRebuyItemDeleteConfirm() { return RANPARAM::bCheckAllDelItemsRebuyPage; }

	// ������ ���� (Ȱ�� ������, ������ ���� ��ġ)
	void BuyItem( DWORD dwPageIndex, WORD wPosX, WORD wPosY, WORD wBuyNum );
	// ������ �Ǹ� (�κ� ��ġ)
	void SellItem( WORD wPosX, WORD wPosY );

	// īƮ�� ������ �ֱ�
	void AddCart( DWORD dwPageIndex, WORD wMID, WORD wSID, WORD wPosX, WORD wPosY, WORD wCount );
	// īƮ�� ������ ����
	void RemoveCart( DWORD dwIndex );

	// īƮ ���� ����
	void RemoveCartAll();
	// īƮ�� ���� ������ ����
	bool BuyCartItem(WORD wTabPage);

	// �����
	bool Rebuy( WORD wIndex );

	// īƮ ����Ʈ
	LuaTable GetCart();
	// īƮ�� �ö� ������ ����
	LONGLONG GetCartTotalPrice() { return m_llTotalCartPrice; }
	// ����� ������ ����Ʈ
	LuaTable GetRebuy();
	// ����� ������ ����
	WORD GetRebuyCount();
	// ���� ������ ID
	DWORD GetAdItemID();
	// ���� ������ �˻���
	std::string GetAdItemSearchString();
	// NPC �̿밡�� üũ;
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

	// ��õ ������ ���� �˻�
	bool CheckRecommendItem( const SITEM* pItemData, const SITEMCUSTOM& sItemCustom,
		bool bTypeCheck, DWORD dwIndex, WORD wPosX, WORD wPosY );
	// ��ȯ ���� �˻�
	bool CheckExchange( const SITEM* pItemData, DWORD dwIndex, WORD wPosX, WORD wPosY );

	// īƮ ������ ���� ���
	void CalcCartItemPrice();

public :
	void SetNpcID( const SNATIVEID& sID ) { m_sNpcID = sID; }
	const SNATIVEID& GetNpcID() { return m_sNpcID; }

	void SetNpcGlobalID( DWORD dwID ) { m_dwNpcGlobalID = dwID; }
	DWORD GetNpcGlobalID() { return m_dwNpcGlobalID; }
};
