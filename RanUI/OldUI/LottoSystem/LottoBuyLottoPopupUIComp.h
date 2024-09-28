#pragma once

#include "../Util/UIGroupHelperMessageBox.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

class CInnerInterface;
class EngieDeviceMan;

class CBasicTextBox;
class RnButton;
class CUIEditBox;
class CUIEditBoxMan;

namespace LottoSystem
{
	class CLottoSystemClient;
}


class CLottoBuyLottoPopupUIComp : public CUIGroupHelperMessageBox
{
public:
	enum
	{
		EAutoInputButton = ID_NEXT + 1,				// 자동 입력 버튼;
		EBuyButton,									// 구입 버튼;
		ECloseButton,								// 닫기 버튼;

		// 가장 아래에 넣어야 함 ( 유동 );
		EEditBoxIdx
	};

	enum
	{
		MESSAGEBOX_NOT_UST = 0,
		MESSAGEBOX_CONFIRM_BUY_TICKET,
		MESSAGEBOX_WARN_INPUT_ZERO,
		MESSAGEBOX_WARN_FAILED_NUM,
		MESSAGEBOX_WARN_INPUT_DUPLICATE,
	};

public:
	CLottoBuyLottoPopupUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~CLottoBuyLottoPopupUIComp();

public:
	virtual void CreateSubControl();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );

	virtual void SetVisibleSingle( BOOL bVisible );

public:
	void UpdateHaveMoney( const LottoSystem::CLottoSystemClient* pLottoSystem );

	void ClearLottoNum();

private:
	void SetLottoNum( const LottoSystem::LOTTO_SYSTEM_LOTTO_NUM& lottoNum,
		LottoSystem::EMLOTTO_BUY_TYPE buyType );

	bool ConfirmInputLottoNum( UIGUID ControlID );

private:
	CBasicTextBox* m_pBuyNumTextBox;

	CBasicTextBox* m_pTicketMoneyTextBox;
	CBasicTextBox* m_pHaveMoneyTextBox;

	CUIEditBoxMan* m_pEditBoxMan;
	std::vector<CUIEditBox*> m_pEditBoxs;

	RnButton* m_pBuyButton;

	CInnerInterface* m_pInterface;
};