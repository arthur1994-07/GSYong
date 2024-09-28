#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class CInnerInterface;
class CItemSlot;
class CBasicScrollBarEx;
class GLInventory;
struct SINVENITEM;
class GLGaeaClient;

class CItemBankPage : public CUIGroup
{
static	const	int	nSTARTLINE;
static	const	int	nOUTOFRANGE;

protected:
	enum
	{
		ITEMBANK_INVEN_X = 6,
		ITEMBANK_INVEN_Y = 32,
		nMAX_ONE_VIEW_SLOT = 8
	};

	enum
	{
		ITEM_SLOT0 = NO_ID + 1,
		ITEM_SLOT_END = ITEM_SLOT0 + ITEMBANK_INVEN_Y
	};

public:
	CItemBankPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CItemBankPage ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );

public:
	void	LoadItemPage ( GLInventory &ItemData );
	void	LoadItem ( SINVENITEM& ref_InvenItem );

	const SINVENITEM* GetItem(int nPosX, int nPosY) const;

public:
	void	UnLoadItemPage ();
	void	UnLoadItem ( int nPosX, int nPosY );

private:
	void	ResetAllItemSlotRender ( int nTotal );
	void	SetItemSlotRender ( int nStartIndex, int nTotal );

private:
	CItemSlot*	CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CUIControl*	CreateControl ( const char* szControl );

private:
	CItemSlot*	m_pItemSlotArray[ITEMBANK_INVEN_Y];
	CUIControl*	m_pItemSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		m_nPosX;
	int		m_nPosY;
	int		m_nCurPos;

protected:
	CInnerInterface* m_pInterface;

};