#pragma once

#include "../Util/UIWindowEx.h"

#include "./ILottoSystemUI.h"

class CInnerInterface;
class EngieDeviceMan;

class CBasicTextBox;
class CLottoWinNumUIComp;
class CLottoWinManListUIComp;

class CSmartComboBoxBasicEx;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 회차의 당첨 결과를 보여주는 UI;
class CLottoWinManListInfo : public CUIWindowEx
{
public:
	enum
	{
		ECloseButton = ET_CONTROL_NEXT + 1,		// 닫기 버튼;
		ETurnComboBox
	};

public:
	CLottoWinManListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	void CreateSubControl();

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );
	virtual void SetFocusControl();
	virtual void ResetFocusControl();

public:
	void ChangeMainTab();

public:
	void UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateWinManList( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

private:
	CBasicTextBox* m_pAccumulateMoneyTextBox;
	CLottoWinNumUIComp* m_pWinNum;
	CLottoWinManListUIComp* m_pWinManListComp;

	CSmartComboBoxBasicEx* m_pComboBox;
	std::vector<unsigned int> m_pComboBoxMemberTurnNum;
};

class MyLottoWinManListInfo : public ILottoWinManListInfo, public CLottoWinManListInfo
{
public:
	MyLottoWinManListInfo( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyLottoWinManListInfo() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual bool IsVisible();

	virtual void ChangeMainTab();

	virtual void UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void UpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void UpdateWinManList( const LottoSystem::CLottoSystemClient* pLottoSystem );
	virtual void UpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
};