//	���� ������
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.6]
//			@ �ۼ�
//

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Inventory/GLInventory.h"

class	CInnerInterface;
class	CItemSlotEx;
class	CBasicScrollBarEx;
struct	SINVENITEM;
class GLGaeaClient;

class CTradeInventoryPage : public CUIGroup
{
private:
static	const	int	nSTARTLINE;
static	const	int	nOUTOFRANGE;

protected:
	enum
	{
		nONE_VIEW_SLOT_DEFAULT = 4,
		nMAX_ONE_VIEW_SLOT = 10
	};

	enum
	{
		ITEM_SLOT0 = NO_ID + 1,
		ITEM_SLOT1,
		ITEM_SLOT_END = ITEM_SLOT0 + EInvenY
	};

public:
	CTradeInventoryPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CTradeInventoryPage ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
	void	UpdateBLOCK ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	void	ResetAllItemSlotRender ( int nTotal );
	void	SetItemSlotRender ( int nStartIndex, int nTotal );

protected:
	CItemSlotEx*	CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CUIControl*	CreateControl ( const char* szControl );

public:
	void	SetOneViewSlot ( const int& nMAX_ONE_VIEW );
	const int& GetOneViewSlot () const						{ return m_nONE_VIEW_SLOT; }

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

public:
	void	ResetAllFlipItem ();
	void	UpdateFlipItem ( GLInventory& ref_TradeInventory );

private:
	CItemSlotEx*	m_pItemSlotArray[EInvenY];
	CUIControl*		m_pItemSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		m_nCurPos;
	int		m_nPosX;
	int		m_nPosY;

private:
	int		m_nONE_VIEW_SLOT;

protected:
	CInnerInterface* m_pInterface;
};