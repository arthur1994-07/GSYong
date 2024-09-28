#pragma once

#include "TexasHoldemWindowDefine.h"

namespace TexasHoldem
{

	class TexasHoldemWindow : public ITexasHoldemWindow, public CUIWindowEx
	{
		enum
		{
			ID_TEXASHOLDEM_BUYCHIP_BUTTON = ET_CONTROL_NEXT+1,
			ID_TEXASHOLDEM_RUNGAME_BUTTON,
			ID_TEXASHOLDEM_MY_CHIP,
		};
	public:
		TexasHoldemWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
		virtual	~TexasHoldemWindow();
	protected:
		GLGaeaClient*		m_pGaeaClient;
		CInnerInterface*	m_pInterface;

		CBasicTextBox*		m_pMyChipText;			//! 보유Chip 텍스트
		RnButton*			m_pBtnBuyChip;			//! Chip구매 버튼컨트롤 포인터
		RnButton*			m_pBtnRunGame;			//! 게임시작 버튼컨트롤 포인터

		UIRECT	m_rcBasicPos;
		UIRECT	m_rcChangePos;

		TexasHoldemExchangeWindow*	m_pExchangeWindow;

	public:
		virtual void CreateUIWindowAndRegisterOwnership(void) override;
		virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) override;
		virtual void SetVisibleSingle( BOOL bVisible ) override;
		virtual void SetVisible( const bool bVisible ) override;
//		virtual void SetNoUpdate( const bool bVisible ) override;
		virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;

		virtual void SetFocusControl();
		virtual void ResetFocusControl();

		void CreateSubControl();

		void OpenExchangeWindow();
		virtual void ClosedExchangeWindow();

	protected:
		inline const bool SetChipMap();
	};

}