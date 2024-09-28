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
			���ӽ��� ��ư Ŭ�� 
			-> GLTexasHoldemClient->StartGame() -> MsgStartGameFB() -> RunTexasHoldemWindow() -> RunTexasHoldemWindow( true )

			WebWindow �������� �� ��� 
			-> GLTexasHoldemClient->EndGame() -> MsgEndGameFB() -> StopTexasHoldemWindow() -> RunTexasHoldemWindow( false )
		*/
		virtual	void RunTexasHoldemWindow ( const bool bVisible )override;

	protected:
		// ���� ó��
		void processCertificate();
		void reset();

	private:
		GLGaeaClient* m_pGaeaClient;
		bool m_bLoadComplete;
	};
}