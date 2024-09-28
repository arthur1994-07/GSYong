#pragma once

#include "./LottoUIDateType.h"

#include "../Util/UIGroupHelper.h"

class CInnerInterface;
class EngineDeviceMan;

class CBasicTextBox;
class CSwapImage;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// ´çÃ· ¹øÈ£ Component;
class CLottoWinNumUIComp : public CUIGroupHelper
{
public:
	CLottoWinNumUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoWinNumUIComp();

public:
	virtual void CreateSubControl();
	virtual	void SetVisibleSingle( BOOL bVisible );

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void SetNumImage( int idx, unsigned int num );

private:
	CInnerInterface* m_pInterface;

	CSwapImage* m_pSlot[ LottoSystem::LOTTO_SLOT_NUM_CONT ];
	CSwapImage* m_pBonusSlot;
};