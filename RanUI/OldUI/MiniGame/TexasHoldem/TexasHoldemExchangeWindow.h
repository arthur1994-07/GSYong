#pragma once

#include "../../Util/TapSelectorEx.h"
#include "TexasHoldemWindowDefine.h"

namespace TexasHoldem
{

	class TexasHoldemExchangeWindow : public CUIWindowEx, private CTapSelectorEx<DEFAULT_RADIO_BUTTON>
	{
	private:
		enum
		{
			ID_TEXASHOLDEM_EXCHANGE_BUYCHIP_BUTTON = ET_CONTROL_NEXT+1,
			ID_TEXASHOLDEM_EXCHANGE_CANCLE_BUTTON,
			ID_TEXASHOLDEM_EXCHANGE_MONEY_RADIO,
			ID_TEXASHOLDEM_EXCHANGE_POINT_RADIO,
			ID_TEXASHOLDEM_EXCHANGE_LIST,
			ID_TEXASHOLDEM_EXCHANGE_MY_MONEY,
			ID_TEXASHOLDEM_EXCHANGE_MSG_BOX,
		};

	public:
		TexasHoldemExchangeWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
		virtual	~TexasHoldemExchangeWindow();

	private:
		GLGaeaClient*		m_pGaeaClient;
		CInnerInterface*	m_pInterface;

		CBasicTextBoxEx*	m_pMyMoneyText;			//! 보유금액 텍스트
		RnButton*			m_pBtnBuyChip;			//! Chip구매 버튼컨트롤 포인터
		RnButton*			m_pBtnCancle;			//! 게임시작 버튼컨트롤 포인터
		CMessageBox*		m_pBuyChipMsgBox;

		DEFAULT_RADIO_BUTTON*		m_pRadioMoney;
		DEFAULT_RADIO_BUTTON*		m_pRadioPoint;

		TexasHoldemExchangeList*	m_pExchangeList;		

		CUIGroup*	m_pCurrentTap;

		D3DXVECTOR2 m_vMoveGap;

	public:
		virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
		virtual	void SetVisibleSingle( BOOL bVisible );
		virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

		void CreateSubControl();

		virtual void	EventSelectedTap( UIGUID ControlID );

	public:
		const bool		SetPurchaseChipList( const std::vector< sChipPrice >& chipMap );

	private:
		void OpenBuyChipMessageBox();
		void RequestExchangeChip();

		bool CheckInvenMoney();

		void ResetList();

	};

}