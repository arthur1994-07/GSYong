#pragma	once

#include "../../Util/WebWindowBase.h"

class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;
class CBasicButton;
class CBasicLineBox;

namespace TexasHoldem
{
	class TexasHoldemWebWindow : public ITexasHoldemWebWindow, public CWebWindowBase
	{
	public:
		TexasHoldemWebWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
		virtual	~TexasHoldemWebWindow ();

	public:
		virtual void CreateUIWindowAndRegisterOwnership(void) override;

		virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )override;
		virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )override;
		virtual	void SetVisibleSingle ( BOOL bVisible ) override;
		virtual void SetURL( const std::string& strURL, const std::string& strPOST );

		/*
			게임시작 버튼 클릭 
			-> GLTexasHoldemClient->StartGame() -> MsgStartGameFB() -> RunTexasHoldemWindow() -> RunTexasHoldemWindow( true )

			WebWindow 닫으려고 할 경우 
			-> GLTexasHoldemClient->EndGame() -> MsgEndGameFB() -> StopTexasHoldemWindow() -> RunTexasHoldemWindow( false )
		*/
		virtual	void RunTexasHoldemWindow ( const bool bVisible )override;

	protected:
		// 인증 처리
		void processCertificate();
		void reset();

	private:
		GLGaeaClient* m_pGaeaClient;
		bool m_bLoadComplete;
	};
}