#pragma once

#include "../Util/UIGroupHelper.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

class CInnerInterface;
class EngineDeviceMan;

class MyLottoBuyLottoUI;
class MyLottoBuyListInfo;
class MyLottoWinManListInfo;

class CBasicTextBox;
class CLottoWinNumUIComp;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 구매 리스트 Component;
class CLottoBuyListUIComp : public CUIGroupHelper
{
public:
	CLottoBuyListUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoBuyListUIComp();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void UpdateBuyList(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int idx = -1 ) = 0;
	virtual void UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;

public:
	inline void SetLottoSystemId( LottoSystem::LOTTO_SYSTEM_ID lottoSystemId ) { m_lottoSystemId = lottoSystemId; }

protected:
	CInnerInterface* m_pInterface;

	CLottoWinNumUIComp* m_pWinNum;

	CBasicTextBox* m_pBuyListTextBox;
	CBasicTextBox* m_pWinNumTextBox;

	LottoSystem::LOTTO_SYSTEM_ID m_lottoSystemId;
};