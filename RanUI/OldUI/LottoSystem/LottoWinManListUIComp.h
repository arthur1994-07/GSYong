#pragma once

#include "../Util/UIGroupHelper.h"

class CInnerInterface;
class EngineDeviceMan;

class CBasicTextBox;

class CLottoWinManListBoxUIComp;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 당첨자 리스트 Component;
class CLottoWinManListUIComp : public CUIGroupHelper
{
public:
	CLottoWinManListUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoWinManListUIComp();

public:
	virtual void CreateSubControl();

public:
	void UpdateWinManList( const LottoSystem::CLottoSystemClient* pLottoSystem );

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	CInnerInterface* m_pInterface;

	CLottoWinManListBoxUIComp* m_pListBox;
};