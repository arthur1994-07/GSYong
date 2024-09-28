#pragma once

#include "./RnPageBase.h"

class CItemSlot;

class CRnPageWish : public CRnPageBase
{
private:
	int m_nMouseX;
	int m_nMouseY;

public:
	CRnPageWish( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void UpdateItemSlot();

	virtual void UIPosToLogicPos( int& nX, int& nY, int nPage );
	virtual void LogicPosToUIPos( int& nX, int& nY, int& nPage );
};