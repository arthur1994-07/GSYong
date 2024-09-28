#pragma once

#include "./RnPageBase.h"

class CItemSlot;

class CRnPageBank : public CRnPageBase
{
public:
	CRnPageBank( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    virtual	void SetVisibleSingle( BOOL bVisible );

	virtual void UIPosToLogicPos( int& nX, int& nY, int nPage );
	virtual void LogicPosToUIPos( int& nX, int& nY, int& nPage );
};