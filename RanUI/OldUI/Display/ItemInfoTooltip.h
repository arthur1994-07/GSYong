#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Item/GLItemBox.h"

#include "../../../RanLogicClient/WishList/WishList.h"

#include "../../RanLogic/Item/ItemSet/GLItemSet.h"

class GLGaeaClient;
class CInnerInterface;
class GLCharacter;

class CBasicLineBoxSmart;
class CItemImage;
class CBasicTextBox;
class CBasicLineBoxEx;
class CBasicVarTextBox;
class CBasicButton;

class CItemInfoTooltip : public CUIGroup
{
private:

	#define EXCHANGE_ITEM_SIZE	10

	enum
	{
		MAIN = NO_ID + 1,
		ITEM_IMAGE,
		CLOSE_BUTTON,
		BOX_ITEM_IMAGES_START,
		BOX_ITEM_IMAGES_END = BOX_ITEM_IMAGES_START + ITEM::SBOX::RANDOM_ITEM_SIZE - 1,
		EXCHANGE_ITEM_IMAGE_START = BOX_ITEM_IMAGES_END + 1,
		EXCHANGE_ITEM_IMAGE_END   = EXCHANGE_ITEM_IMAGE_START + EXCHANGE_ITEM_SIZE - 1,
	};

	enum ITEM_TOOLTIP_STATE_BLOW
	{
		ITEM_TOOLTIP_VALUE = 0,
		ITEM_TOOLTIP_VALUE_COUNT_1_VAR1 = 1,
		ITEM_TOOLTIP_VALUE_COUNT_1_VAR2 = 2,
		ITEM_TOOLTIP_VALUE_COUNT_2 = 3,
	};

public:
	CItemInfoTooltip( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CItemInfoTooltip();

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;
	GLCharacter* m_pCharacter;

private:
	CBasicLineBoxSmart* m_pLineBox;
	CUIControl* m_pDummyBase;
	CUIControl* m_pDummyControl;
	CUIControl* m_pDummyNameControl;
	CUIControl* m_pDummyLabel5Control;

	CItemImage* m_pItemImage;
	CBasicTextBox* m_pItemName;
	CBasicTextBox* m_pItemLabel1;
	CBasicTextBox* m_pItemLabel2;
	CBasicTextBox* m_pItemLabel3;
	CBasicTextBox* m_pItemLabel4;
	CBasicTextBox* m_pItemLabel5;
	CBasicTextBox* m_pItemLabel6; // 교환아이템
	CBasicTextBox* m_pItemSet;	//셋트아이템
	
	CUIControl* m_pWishListLine;
	CBasicTextBox* m_pWishListItem;

	CUIControl* m_pResistSpirit;
	CUIControl* m_pResistFire;
	CUIControl* m_pResistIce;
	CUIControl* m_pResistElectric;
	CUIControl* m_pResistPoison;

	CBasicTextBox* m_pResistSpiritText;
	CBasicTextBox* m_pResistFireText;
	CBasicTextBox* m_pResistIceText;
	CBasicTextBox* m_pResistElectricText;
	CBasicTextBox* m_pResistPoisonText;

	CBasicLineBoxSmart* m_pLink_LineBox;
	CBasicButton* m_pLink_Close;

	CItemImage* m_pBoxItemImages[ITEM::SBOX::RANDOM_ITEM_SIZE];
	CItemImage* m_pExchangeItemImages[EXCHANGE_ITEM_SIZE];

	CUIControl* m_pColorBox;

private:
	bool m_bLinkBoxStyle;
	SITEM* m_pItemData;
	D3DXVECTOR2 m_vMousePointGap;
	bool m_bDisplayResist;

	bool m_bShopOpen;
	bool m_bInMarket;
	bool m_bInPrivateMarket;
	bool m_bUsePreview;
	bool m_bIsWEAR_ITEM;
	bool m_bIsDurableTip;

	int  m_iPageIndex;
	WORD m_wPosX;
	WORD m_wPosY;
	SNATIVEID m_sNpcNativeID;

	WishList::ItemSPtr m_pWishItem;
	SITEMCUSTOM m_sItemcustom;

	int m_PosX;
	int m_PosY;
	D3DXVECTOR2 m_vDragPos;
	bool m_bBlockMousetracking;

	int m_nMouseX;
	int m_nMouseY;

	bool m_bGroup;

	float m_fDurabilityValue[DURABILITY_APPLY_COUNT];
public:
	void CreateSubControl();
	void CreateLinkBoxStyle();
	void CreateCloseButton();

	void AllControlRePos( int x, int y );

	void DisplayItemInformation(const SITEMCUSTOM& sItemCustom, const BOOL bShopOpen,
		const BOOL bInMarket, const BOOL bInPrivateMarket, const BOOL bUsePreview,
		const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY, SNATIVEID sNpcNativeID, int iPageIndex = -1 );
	void DisplayWishListItemInfo( WishList::ItemSPtr pWishItem );
	void DisplayWishListItemInfo( SITEMCUSTOM& sItemcustom );
	void ResetInformation();

	void DisplayItemDurability(const SITEMCUSTOM& sItemCustom, const BOOL bShopOpen,
		const BOOL bInMarket, const BOOL bInPrivateMarket, const BOOL bUsePreview,
		const BOOL bIsWEAR_ITEM, const BOOL bIsDurable, WORD wPosX, WORD wPosY, SNATIVEID sNpcNativeID, int iPageIndex = -1);


private:
	void SetItemImage( const SITEMCUSTOM& sItemCustom );
	void SetItemName( const SITEMCUSTOM& sItemCustom );
	void SetItemName( const char* pName );
	void SetInformationLabel1( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabel2( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabel3( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabel3_RandomOption( int nRandOptType, float fVal, bool bIsPerRandOpt, DWORD dwAddAbility );
	void SetInformationLabel4( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabel5( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabel6( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabelSetItem( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabelSetItemName( const SSETITEM *sSetItem, const SSETITEMSEARCHINFO &sInfoPutOnSetItem );
	void SetInformationLabelSetItemParts( const SSETITEM *sSetItem, const SSETITEMSEARCHINFO &sInfoPutOnSetItem );
	void SetInformationLabelSetItemBonus( const SSETITEM *sSetItem, const SSETITEMSEARCHINFO &sInfoPutOnSetItem );
	void SetInformationLabelSetItemPreview(void);

	void DisplaySkillItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom );
	void DisplayPetSkillItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom );
	void DisplayVehicleItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom );
	void DisplayPetCardItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom );
	void DisplayLunchBoxItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom );
	void DisplayPetDualSkillItem( CBasicTextBox* m_pTextBox, const SITEMCUSTOM& sItemCustom );
	void DisplayWishListInfo();

	void SetInformationLabel2_cs( const SITEMCUSTOM& sItemCustom );
	void SetInformationLabel3_cs( const SITEMCUSTOM& sItemCustom );

	void ResetDurabilityValue( float fResetValue = 1.0f );
public:
	void ShowColorBox();
	void HideColorBox();
	void SetBlockMouseTracking( bool bTracking ) { m_bBlockMousetracking = bTracking; }
	const UIRECT& GetBasePos() { return m_pDummyBase->GetLocalPos(); }
	void SetWishListItem( WishList::ItemSPtr& _pWishItem) { m_pWishItem = _pWishItem; }
	WishList::ItemSPtr GetWishListItem() { return m_pWishItem; }
	void SetSItemcustomList( const SITEMCUSTOM& _sItemcustom ) { m_sItemcustom = _sItemcustom; }
	SITEMCUSTOM GetSItemcustomList() {return m_sItemcustom; }
	void CutZero( CString& cstr, int ncount );// 소수점이 있는 숫자 표현시 뒤에 유효하지 않는 '0'을 제거
	void SetBGAlpa(WORD wValue);

	void SetGroup();

public:
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible ) override;
	void TranslateMessage( DWORD dwMsg );

private:
	float m_fDurabilityPerformance;
	float m_pResistTextGap;
};
