#pragma once

#include "../Util/UIGroupHelper.h"

class CInnerInterface;
class EngineDeviceMan;

class MyLottoBuyLottoUI;
class MyLottoBuyListInfo;
class MyLottoWinManListInfo;

class CBasicTextBox;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// √ﬂ√∑ ¿œΩ√ Component;
class CLottoDrawingDateUIComp : public CUIGroupHelper
{
public:
	CLottoDrawingDateUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoDrawingDateUIComp();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

public:
	inline CBasicTextBox* GetDateTextBox() { return m_pDateTextBox; }
	inline CBasicTextBox* GetStartTimeTextBox() { return m_pStartTimeTextBox; }
	inline CBasicTextBox* GetEndTimeTextBox() { return m_pEndTimeTextBox; }
	inline CBasicTextBox* GetProgressTimeTextBox() { return m_pProgressTimeTextBox; }
	inline CBasicTextBox* GetBuyStateTextBox() { return m_pBuyStateTextBox; }

private:
	CInnerInterface* m_pInterface;

	// Text Box;
	CBasicTextBox* m_pDateTextBox;
	CBasicTextBox* m_pStartTimeTextBox;
	CBasicTextBox* m_pEndTimeTextBox;
	CBasicTextBox* m_pProgressTimeTextBox;
	CBasicTextBox* m_pBuyStateTextBox;
};