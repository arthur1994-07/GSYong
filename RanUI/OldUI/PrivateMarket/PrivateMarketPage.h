//	마켓 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.6]
//			@ 작성
//

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Inventory/GLInventoryPrivateMarket.h"

class	CInnerInterface;
class	CFlipItemSlot;
class	CBasicScrollBarEx;
class	GLInventory;
struct	SINVENITEM;
class	GLPrivateMarket;
class	GLGaeaClient;

class CPrivateMarketPage : public CUIGroup
{
private:
static	const	int	nSTARTLINE;
static	const	int	nOUTOFRANGE;

protected:
	enum
	{
		nMAX_ONE_VIEW_SLOT = 4
	};

	enum
	{
		ITEM_SLOT0 = NO_ID + 1,
		ITEM_SLOT1,
		ITEM_SLOT2,
		ITEM_SLOT3,
		ITEM_SLOT4,
		ITEM_SLOT5,
		ITEM_SLOT6,
		ITEM_SLOT7,
		ITEM_SLOT_END = ITEM_SLOT0 + EPrivateMarketY
	};

public:
	CPrivateMarketPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPrivateMarketPage ();

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );

public:
	void	LoadItemPage ( GLInventory &ItemData, bool bOPENER );
	void	LoadItem ( SINVENITEM& ref_InvenItem );

	const SINVENITEM* GetItem(int nPosX, int nPosY) const;

public:
	void	UnLoadItemPage ();
	void	UnLoadItem ( int nPosX, int nPosY );

	void	DoFlipItem ( WORD wPosX, WORD wPosY, BOOL bFlip );
	void	DoFlipItemPage ( GLPrivateMarket &sPMarket );

	BOOL	IsFlipItem ( WORD wPosX, WORD wPosY );

private:
	void	ResetAllItemSlotRender ( int nTotal );
	void	SetItemSlotRender ( int nStartIndex, int nTotal );

private:
	CFlipItemSlot*	CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CUIControl*	CreateControl ( const char* szControl );

private:
	CFlipItemSlot*	m_pItemSlotArray[EPrivateMarketY];
	CUIControl*	m_pItemSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		m_nPosX;
	int		m_nPosY;
	int		m_nCurPos;

private:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};
