#pragma once

#include "./LottoBuyListUIComp.h"

class CInnerInterface;
class EngineDeviceMan;

class CLottoConfirmBuyListBoxUIComp;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 구매 리스트 Component;
class CLottoConfirmBuyListUIComp : public CLottoBuyListUIComp
{
public:
	CLottoConfirmBuyListUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoConfirmBuyListUIComp();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void UpdateBuyList(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int idx = -1 );
	virtual void UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem );

public:
	void ConfirmRanking();
	void ConfirmStateOff();

	void CloseUI();

private:
	CLottoConfirmBuyListBoxUIComp* m_pListBox;

	bool m_bClickConfrim;
};