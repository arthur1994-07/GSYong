#pragma	once

#include "../Util/UIWindowEx.h"
#include "InventoryPage.h" // by luxes.
#include "../../InnerInterface.h"

class	CInnerInterface;
class	CInventoryPage;
class	CInventoryPageWear;
class	CInventoryPageWearEx;
class GLGaeaClient;
class   CBasicTextButton;

class CInventoryWindow : public CUIWindowEx
{
protected:
	enum
	{
		INVENTORY_MONEY_BUTTON = ET_CONTROL_NEXT,
		INVENTORY_PAGE,
		INVENTORY_PAGEWEAR,
		INVENTORY_PAGEWEAR_EX,
		INVENTORY_VNGAINSYS_BUTTON,
        INVENTORY_ITEM_SORT_BUTTON,
	};

private:
	CInventoryPage*		m_pPage;
	CInventoryPageWear*	m_pPageWear;
	CInventoryPageWearEx* m_pPageWearEx;

	CBasicTextBox*		m_pMoneyTextBox;
    CBasicTextButton*   m_pItemSortButton;

	WORD	m_wSplitItemPosX;
	WORD	m_wSplitItemPosY;
	BOOL	m_bSplitItem;

	BOOL	m_bLock;

	int		m_nONE_VIEW_SLOT;

public:
	CInventoryWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CInventoryWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

public:
	void	GetSplitPos( WORD* pwPosX, WORD* pwPosY );
	void	SetSplitPos( WORD wPosX, WORD wPosY );

	void	SetArmSwapTabButton( BOOL bArmSub );

	void	SetLock() { m_bLock = TRUE;  }
	void	SetUnLock() { m_bLock = FALSE; }
    const bool IsLocked () { return m_bLock ? true : false; }

	const UIRECT&	GetInventoryGlobalPos() { return m_pPage->GetGlobalPos(); } // by luxes.

public:
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	void	SetMoney( LONGLONG Money );

public:
	void	SetOneViewSlot( const int& nONE_VIEW_SLOT );
	const int& GetOneViewSlot() const						{ return m_nONE_VIEW_SLOT; }

};

class MyInventoryWindow : public IInventoryWindow, private CInventoryWindow
{
public:
    MyInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyInventoryWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetOneViewSlot( const int& nONE_VIEW_SLOT );
    virtual const int& GetOneViewSlot() const;
    virtual void SetArmSwapTabButton( BOOL bArmSub );
    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual const UIRECT& GetInventoryPageGlobalPos();
    virtual void LockTrade();
    virtual void UnLockTrade();
    virtual const bool IsLocked();
    virtual void GetSplitPos( WORD* pwPosX, WORD* pwPosY );
};