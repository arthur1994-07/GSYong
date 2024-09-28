#pragma once

#include "../Util/UIGroupHelper.h"

class CInnerInterface;
class EngineDeviceMan;

class CBasicTextBox;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 누적 당첨금 Component;
class CLottoAccumulateMoneyUIComp : public CUIGroupHelper
{
public:
	CLottoAccumulateMoneyUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoAccumulateMoneyUIComp();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void UpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

private:
	CInnerInterface* m_pInterface;

	// 누적 당첨금 관련 UI;
	CBasicTextBox* m_pAccumulateMoneyTextTextBox;
	CBasicTextBox* m_pAccumulateMoneyTextBox;
};