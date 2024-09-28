#pragma once

#include "../../../EngineLib/GUInterface/UIWindow.h"

struct SINVENITEM;

class IInventory
{
public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;
	virtual void CallTapSelect( UIGUID controlID ) = 0;
	virtual UIGUID GetTapSelected() = 0;

	virtual const UIRECT& GetUIWindowGlobalPos() = 0;
    virtual const UIRECT& GetInventoryPageGlobalPos() = 0;

	virtual void LockTrade() = 0;
    virtual void UnLockTrade() = 0;
    virtual const bool IsLocked() = 0;

	virtual void LockInven( UIGUID ControlID ) = 0;
	virtual void UnLockInven( UIGUID ControlID ) = 0;

	virtual const SINVENITEM* GetFocusItem() = 0;
	virtual void GetConsumeSplitInvenPos( WORD& nPosX, WORD& nPosY ) = 0;

	virtual bool IsCurrentInvenPage( int& nX, int& nY ) = 0;
	virtual void ChangeInvenPage( int& nX, int& nY ) = 0;
	virtual void SetBorderItemSlot( int nX, int nY ) = 0;
	virtual void ResetBorderItemSlot() = 0;
	virtual bool IsItemDecompose() = 0;
	virtual void GetItemDecomposePos(WORD& nPosX, WORD& nPosY) = 0;
	virtual void SetWindowOpenBox( int nX, int nY, BOOL bShow = FALSE ) = 0;
};