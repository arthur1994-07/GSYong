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
#include "../../../RanLogic/Trade/GLTrade.h"

class	CInnerInterface;
class	CItemSlotEx;
class	GLInventory;
class	GLGaeaClient;
struct	SINVENITEM;

class CTradePage : public CUIGroup
{
private:
static	const	int	nOUTOFRANGE;

private:
	enum
	{		
		nMAX_ONE_VIEW_SLOT = 2
	};

	enum
	{
		ITEM_SLOT0 = NO_ID + 1,
		ITEM_SLOT1,
		ITEM_SLOT_END = ITEM_SLOT0 + ETradeY
	};


public:
	CTradePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CTradePage ();

public:
	void	CreateSubControl ();

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

public:	
	void	SetAllFlipItem ( BOOL bFlip );	

private:
	CItemSlotEx*	CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CUIControl*	CreateControl ( char* szControl );

private:
	CItemSlotEx*	m_pItemSlotArray[ETradeY];

private:
	int		m_nPosX;
	int		m_nPosY;

protected:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};
