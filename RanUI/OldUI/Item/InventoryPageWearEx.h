#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../Feature/CostumColorInfo.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_WEARSLOTEX = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CInnerInterface;
class CItemImage;
class CBasicTextButton;
class GLGaeaClient;

class CInventoryPageWearEx : public CUIGroup
{
private:
static	const	int	nOUTOFRANGE;

private:
	enum
	{
		ITEM_IMAGE0 = NO_ID + 1,
		ITEM_IMAGE1,
		ITEM_IMAGE2,
		ITEM_IMAGE3,
		ITEM_IMAGE4,
		ITEM_IMAGE5,
		ITEM_IMAGE6,
		ITEM_IMAGE7,
		ITEM_IMAGE8,
		ITEM_IMAGE9,
		ITEM_IMAGE10,
		ITEM_IMAGE11,
		ITEM_IMAGE12,
		ITEM_IMAGE13,
        ITEM_IMAGE14,
        ITEM_IMAGE15,
        ITEM_IMAGE16,
        ITEM_IMAGE17,
        ITEM_IMAGE18,

		ITEM_IMAGE_SIZE = 19,

		TEXT_A_BUTTON,
		TEXT_B_BUTTON
	};

	enum
	{
		ITEM_COLOR_INFO0 = ITEM_IMAGE_SIZE + 2 + 1,
		ITEM_COLOR_INFO1,
		ITEM_COLOR_INFO2,
		ITEM_COLOR_INFO3,
		ITEM_COLOR_INFO4,
		ITEM_COLOR_INFO5,
		ITEM_COLOR_INFO6,
		ITEM_COLOR_INFO7,
		ITEM_COLOR_INFO8,
		ITEM_COLOR_INFO9,
		ITEM_COLOR_INFO10,
		ITEM_COLOR_INFO11,
        ITEM_COLOR_INFO12,
        ITEM_COLOR_INFO13,
        ITEM_COLOR_INFO14,
        ITEM_COLOR_INFO15,
        ITEM_COLOR_INFO16,
        ITEM_COLOR_INFO17,
        ITEM_COLOR_INFO18,
	};

public:
	CInventoryPageWearEx(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CInventoryPageWearEx ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ( int nClassType );

private:
	CUIControl*	CreateControl ( const char* szControl );
	CItemImage*	CreateItemImage ( const char* szControl, UIGUID ControlID );
	CCostumColorInfo*	CreateColorInfo( const char* szControl, UIGUID ControlID ); // by luxes.

public:
	void	SetItemIndex ( int nIndex )				{ m_nIndex = nIndex; }
	int		GetItemIndex ()							{ return m_nIndex; }

	EMSLOT	GetItemSlot ();

public:
	void	LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom );
	SITEMCUSTOM&	GetItem ( int nIndex );
	void	UnLoadItem ( int nIndex );

	void SetTabButton( BOOL bMain ); // 무기 스왑

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	EMSLOT	IMAGE2EMSLOT ( int nIndex );

private:
	CUIControl*		m_pMouseOver;

	CBasicTextButton* m_pAButton;
	CBasicTextButton* m_pBButton;

	CUIControl*		m_pARHandSlot;
	CUIControl*		m_pALHandSlot;
	CUIControl*		m_pBRHandSlot;
	CUIControl*		m_pBLHandSlot;

private:
	CUIControl*	m_pSlotDisplay[ITEM_IMAGE_SIZE];
	CItemImage*	m_pItemImage[ITEM_IMAGE_SIZE];
	SITEMCUSTOM	m_ItemCustomArray[ITEM_IMAGE_SIZE];
	CCostumColorInfo*	m_pColorInfo[ ITEM_IMAGE_SIZE ]; // by luxes.

private:
	int		m_nIndex;	

protected:
	CInnerInterface* m_pInterface;

};