#pragma once

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

#include "./InventoryInterface.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;
class CRnPageBase;
class CRnPageInven;
class CRnPageBank;
class CRnPageWish;

class CRnInventoryWindow : public CUIWindowEx, private CTapSelectorEx< DEFAULT_TAPBUTTON >, public IInventory
{
public:
	typedef CTapSelectorEx::BUTTON STAPBUTTON;

public:
	enum
	{
		ETapInven = ET_CONTROL_NEXT + 1,
		ETapBank,
		ETapWish,

		EPageInven,
		EPageBank,
		EPageWish,
	};

private:
	GLGaeaClient* m_pGaeaClient;

	CRnPageBase* m_pCurrent;
	CRnPageInven* m_pInven;
	CRnPageBank* m_pBank;
	CRnPageWish* m_pWish;

public:
	CRnInventoryWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	void CreateSubControl();

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void SetFocusControl();


public:
	virtual void CallTapSelect( UIGUID controlID );
	virtual UIGUID GetTapSelected();

	virtual const UIRECT& GetUIWindowGlobalPos();
    virtual const UIRECT& GetInventoryPageGlobalPos();

	virtual void LockTrade();
    virtual void UnLockTrade();
    virtual const bool IsLocked();

	virtual void LockInven( UIGUID ControlID );
	virtual void UnLockInven( UIGUID ControlID );

	virtual const SINVENITEM* GetFocusItem();
	virtual void GetConsumeSplitInvenPos( WORD& nPosX, WORD& nPosY );

	virtual bool IsCurrentInvenPage( int& nX, int& nY );
	virtual void ChangeInvenPage( int& nX, int& nY );
	virtual void SetBorderItemSlot( int nX, int nY );
	virtual void ResetBorderItemSlot();
	virtual bool IsItemDecompose();
	virtual void GetItemDecomposePos(WORD& nPosX, WORD& nPosY);
	virtual void SetWindowOpenBox( int nX, int nY, BOOL bShow = FALSE );

private:
	void EventSelectedTap( UIGUID controlID );
};