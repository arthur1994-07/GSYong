#pragma once

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

#include "../Util/UIGroupHelperMessageBox.h"

class CInnerInterface;
class EngineDeviceMan;

class MyLottoBuyLottoUI;
class MyLottoBuyListInfo;
class MyLottoWinManListInfo;

class CLottoAccumulateMoneyUIComp;
class CLottoDrawingDateUIComp;
class CLottoBasicBuyListUIComp;
class CLottoMachineViewerUIComp;

class CBasicTextBox;
class CCheckBox;
class RnButton;
class CBasicLineBoxSmart;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// Lotto System Main UI �ǵ��� ���� UI;
class CMainUIPageBase : public CUIGroupHelperMessageBox
{
protected:
	enum
	{
		EUpdateAccumulateMoneyButton = ID_NEXT,		// ���� ��÷�� ���� ��ư;
		EViewAccumulateMoneyCheckBox,				// ���� ��÷�� �׻� ���� üũ�ڽ�;
		EViewWinManListButton,						// ���� ȸ ��� ���� ��ư;
		EConfirmMyResultButton,						// ��÷ Ȯ�� ��ư;
		ECloseButton								// �ݱ� ��ư;
	};

public:
	CMainUIPageBase(
		CInnerInterface* pInterface,
		EngineDeviceMan* pEngineDevice );
	virtual ~CMainUIPageBase();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg ){};

public:
	virtual void SetVisibleSingle( BOOL bVisible );

protected:
	void ConfirmMyResultButtonProc( UIGUID ControlID, DWORD dwMsg );

public:
	void InactiveConfrimButton();
	void ActiveConfrimButton();
	
	void SetSystemID( LottoSystem::LOTTO_SYSTEM_ID systemId );

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
	void UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem, int idx = -1 );
	void UpdateBuyListCount( const LottoSystem::CLottoSystemClient* pLottoSystem, int count );
	void UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );

	void UpdateMachineAnimation( float fElapsedTime );

public:
	inline CLottoAccumulateMoneyUIComp* GetAccumulateMoneyUIComp() { return m_pAccumulateMoneyUIComp; }
	inline CLottoBasicBuyListUIComp* GetBuyListUIComp() { return m_pBuyListUIComp; }
	inline CLottoDrawingDateUIComp* GetDrawingDateUIComp() { return m_pDrawingDateUIComp; }
	inline CLottoMachineViewerUIComp* GetMachineViewerUIComp() { return m_pMachineViewerUIComp; }

protected:
	CInnerInterface* m_pInterface;

	CCheckBox* m_pCheckBox;
	RnButton* m_pUpdateButton;
	RnButton* m_pConfirmWinButton;

	// Lotto System ID;
	LottoSystem::LOTTO_SYSTEM_ID m_systemId;

	// ���� ��÷�� UI;
	CLottoAccumulateMoneyUIComp* m_pAccumulateMoneyUIComp;

	// ���� ��� �� ��÷ ��ȣ UI;
	CLottoBasicBuyListUIComp* m_pBuyListUIComp;

	// ��÷�� UI;
	CLottoMachineViewerUIComp* m_pMachineViewerUIComp;

	// ��÷ �Ͻ� UI;
	CLottoDrawingDateUIComp* m_pDrawingDateUIComp;
};