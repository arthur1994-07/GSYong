#pragma once

#include "../Util/UIGroupHelperNonClose.h"
#include "../../InnerInterface.h"

#include "./ILottoSystemUI.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

class CInnerInterface;
class EngieDeviceMan;

class CMoveableTextBox;

// 로또 시스템의 누적 당첨금을 알리는 전광판 역할;
class CLottoDisplayUI : public CUIGroupHelperNonClose
{
public:
	CLottoDisplayUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoDisplayUI();

	void CreateSubControl();

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void ResetTextAnimation();

protected:
	CMoveableTextBox* m_pMoveableTextBox;

	float m_fTime;
};

class MyLottoDisplayUI : public ILottoDisplayUI, public CLottoDisplayUI
{
public:
	MyLottoDisplayUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyLottoDisplayUI() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual bool IsVisible();

	virtual void UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void ResetTextAnimation();
};