#pragma once

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"

#include "../Util/MessageBox.h"

#include "./ILottoSystemUI.h"

class CInnerInterface;
class EngieDeviceMan;

class CMainUIPageBase;

// 로또 시스템의 Main UI;
class CLottoMainUI : public CUIWindowEx, private CTapSelectorEx< DEFAULT_TAPBUTTON >
{
public:
	typedef CTapSelectorEx::BUTTON STAPBUTTON;

public:
	enum
	{
		EViewHelpButton = ET_CONTROL_NEXT + 1,
		ETabID,
		EPageID = ETabID + LottoSystem::LOTTO_MAX_PAGE_SIZE,
	};

public:
	CLottoMainUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	void CreateSubControl();

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual void SetFocusControl();

private:
	virtual void EventSelectedTap( UIGUID controlID );

public:
	void InitializeUI( const LottoSystem::CLottoSystemClient* pLottoSystem );

public:
	void UpdateAccumulateMoneyTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

	void UpdateDrawingDateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateBuyListStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

	void UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

	void UpdateMachineTurn( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateMachine(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int drawingNum );
	void UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateBuyList(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int idx = -1 );
	void UpdateBuyListCount(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int count );
	void UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );

	void UpdateMachineAnimation(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		float fElapsedTime );

public:
	void OpenMessageBox(
		const CMessageBox::BOXTITLE eBoxTitle,
		const std::string& strMessage, 
		const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OK,
		unsigned int nIdentity = 0 );

	void InactiveConfirmButton();
	void ActiveConfirmButton();

private:
	CMainUIPageBase* m_pCurrentPage;

	STAPBUTTON*	m_pTab[ LottoSystem::LOTTO_MAX_PAGE_SIZE ];
	CMainUIPageBase* m_pPage[ LottoSystem::LOTTO_MAX_PAGE_SIZE ];

	CBasicLineBoxSmart* m_pHelpBox;
	CBasicTextBox* m_pHelpText[ LottoSystem::HELP_TEXT_COUNT ];
};

class MyLottoMainUI : public ILottoMainUI, public CLottoMainUI
{
public:
	MyLottoMainUI( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyLottoMainUI() { };

public:
	 virtual void CreateUIWindowAndRegisterOwnership();

	 virtual bool IsVisible();

	 virtual void InitializeUI( const LottoSystem::CLottoSystemClient* pLottoSystem );

	 virtual void UpdateAccumulateMoneyTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

	 virtual void UpdateDrawingDateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	 virtual void UpdateBuyListStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

	 virtual void UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	 virtual void UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

	 virtual void UpdateMachineTurn( const LottoSystem::CLottoSystemClient* pLottoSystem );
	 virtual void UpdateMachine(
		 const LottoSystem::CLottoSystemClient* pLottoSystem,
		 int drawingNum );
	 virtual void UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem );
	 virtual void UpdateBuyList(
		 const LottoSystem::CLottoSystemClient* pLottoSystem,
		 int idx = -1 );
	 virtual void UpdateBuyListCount(
		 const LottoSystem::CLottoSystemClient* pLottoSystem,
		 int count );
	 virtual void UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );

	 virtual void UpdateMachineAnimation(
		 const LottoSystem::CLottoSystemClient* pLottoSystem,
		 float fElapsedTime );

	 virtual void OpenMessageBox(
		 const CMessageBox::BOXTITLE eBoxTitle,
		 const std::string& strMessage, 
		 const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OK,
		 unsigned int nIdentity = 0 );

	 virtual void InactiveConfirmButton();
	 virtual void ActiveConfirmButton();
};