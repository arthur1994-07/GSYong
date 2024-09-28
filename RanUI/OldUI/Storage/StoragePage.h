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
#include "../../../RanLogic/Character/GLCharDefine.h"

class CInnerInterface;
class CItemSlot;
class CBasicScrollBarEx;
class GLInventory;
struct SINVENITEM;
class GLGaeaClient;

class CStoragePage : public CUIGroup
{
private:
static	const	int	nSTARTLINE;
static	const	int	nOUTOFRANGE;

protected:
	enum
	{
		nMAX_ONE_VIEW_SLOT = 8
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
		ITEM_SLOT_END = ITEM_SLOT0 + EStorageY
	};

public:
	CStoragePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CStoragePage();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	InitStoragePage ( int nPage );

public:
	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );

public:
	void	LoadItemPage ( GLInventory &ItemData, const bool bUSABLE = false );
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
	CItemSlot*	m_pItemSlotArray[EStorageY];
	CUIControl*	m_pItemSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		m_nPage;

private:
	int		m_nCurPos;
	int		m_nPosX;
	int		m_nPosY;

private:
	CInnerInterface* m_pInterface;

};

/////////////////////////////////////////////////////////////////////////////////////
//#pragma	once
//
//#include "UIGroup.h"
//#include "../../../EngineLib/G-Logic/GLDefine.h"
//#include "../../../RanLogic/Character/GLCharDefine.h"
//
//class	CItemSlot;
//class	CBasicScrollBarEx;
//class	GLInventory;
//struct	SINVENITEM;

class CClubStoragePage : public CUIGroup
{
private:
static	const	int	nSTARTLINE;
static	const	int	nOUTOFRANGE;

protected:
	enum
	{
		nMAX_ONE_VIEW_SLOT = 8
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
		ITEM_SLOT_END = ITEM_SLOT0 + EStorageY
	};

public:
	CClubStoragePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CClubStoragePage();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	InitStoragePage ( int nPage );

public:
	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );

public:
	void	LoadItemPage(GLInventory* pItemData, const bool bUSABLE = false);
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
	CItemSlot*	m_pItemSlotArray[EStorageY];
	CUIControl*	m_pItemSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		m_nPage;

private:
	int		m_nCurPos;
	int		m_nPosX;
	int		m_nPosY;

protected:
	CInnerInterface* m_pInterface;

};