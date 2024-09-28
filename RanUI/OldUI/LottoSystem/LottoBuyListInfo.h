#pragma once

#include "../Util/UIWindowEx.h"

#include "./ILottoSystemUI.h"

class CInnerInterface;
class EngieDeviceMan;

class CLottoConfirmBuyListUIComp;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 전회 구매리스트의 당첨 확인을 위한 UI;
class CLottoBuyListInfo : public CUIWindowEx
{
public:
	enum
	{
		EConfirmButton = ET_CONTROL_NEXT + 1,	// 확인 하기 버튼;
		ECloseButton							// 닫기 버튼;
	};

public:
	CLottoBuyListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~CLottoBuyListInfo();

	void CreateSubControl();

public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );

public:
	// Main Tab이 변경 될 때 갱신되어야 할 UI;
	void ChangeMainTab();

public:
	// Update 로직;
	void UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem,
		int idx = -1 );
	void UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem );

public:
	// 확인하기 버튼 On/Off;
	void ConfirmRanking();
	void ConfirmStateOff();

public:
	inline CLottoConfirmBuyListUIComp* GetBuyListUIComp() { return m_pBuyListUIComp; }

private:
	CLottoConfirmBuyListUIComp* m_pBuyListUIComp;
};

class MyLottoBuyListInfo : public ILottoBuyListInfo, public CLottoBuyListInfo
{
public:
	MyLottoBuyListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyLottoBuyListInfo() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual bool IsVisible();

	virtual void ChangeMainTab();

	virtual void UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem );

	virtual void ConfirmRanking();
	virtual void ConfirmStateOff();
};