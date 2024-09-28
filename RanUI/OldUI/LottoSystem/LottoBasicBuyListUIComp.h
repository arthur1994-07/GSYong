#pragma once

#include "./LottoBuyListUIComp.h"

class CInnerInterface;
class EngineDeviceMan;

class CLottoBasicBuyListBoxUIComp;

namespace LottoSystem
{
	class CLottoSystemClient;
}

class CLottoBasicBuyListUIComp : public CLottoBuyListUIComp
{
public:
	CLottoBasicBuyListUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoBasicBuyListUIComp();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void SetVisibleSingle( BOOL bVisible );

public:
	virtual void UpdateBuyList(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int idx = -1 );
	virtual void UpdateBuyListCount(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int count );
	virtual void UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem );
	
private:
	CLottoBasicBuyListBoxUIComp* m_pListBox;
};