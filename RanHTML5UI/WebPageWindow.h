#pragma once
#include "../=Html5UI_Interface/GUIBase/IWindow.h"
#include "../=Html5UI_Interface/GUIBase/UIEvent.h"
//#include <GUIBase/IWindow.h>
//#include <GUIBase/UIEvent.h>
#include "UIWebView.h"

class GLGaeaClient;
class EngineDeviceMan;

namespace HTML5UI
{

	class CWebPageWindow : public GUIBase::IWindow
	{
	public:
		CWebPageWindow( GUIBase::UIID uiID
			, const std::string& uiStrID
			, GUIBase::UIRECT rcParentGlobalPos
			, GUIBase::WINDOW_CFG windowCFG
			, GUIBase::IWebView* webView
			, GLGaeaClient* pGaeaClient
			, EngineDeviceMan* pEngineDevice );
		~CWebPageWindow();

	public:
		/// ÃÊ±âÈ­ & ÆÄ±«;
		virtual void Initialize();
		virtual void Destroy();

		virtual bool OnMouseEvent( std::tr1::shared_ptr<GUIBase::CUIMouseEvent> mouseEvent );

		/// Loop
		virtual bool FrameMove( float timeDelta );

		/// MsgProc
		virtual bool MsgProc( const GUIBase::CALLBACK_MESSAGE msg );

	private:
		GLGaeaClient* m_pGaeaClient;
		EngineDeviceMan* m_pEngineDevice;
	};

}