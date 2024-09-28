#pragma once

#include "../Util/UIWindowEx.h"

#include "./ILottoSystemUI.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

class CInnerInterface;
class EngieDeviceMan;

class CLottoBuyLottoPopupUIComp;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 로또 구매를 위한 UI;
class CLottoBuyLottoUI : public CUIWindowEx
{
public:
	CLottoBuyLottoUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	void CreateSubControl();

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	virtual void SetVisibleSingle( BOOL bVisible );

public:
	void UpdateHaveMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );

public:
	CLottoBuyLottoPopupUIComp* m_pPopup;
};

class MyLottoBuyLottoUI : public ILottoBuyLottoUI, public CLottoBuyLottoUI
{
public:
	MyLottoBuyLottoUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyLottoBuyLottoUI() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual bool IsVisible();

	virtual void UpdateHaveMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );
};