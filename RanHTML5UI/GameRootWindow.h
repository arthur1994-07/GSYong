#pragma once

#include <GUIBase/IWindow.h>
#include <GUIBase/UIEvent.h>
#include "UIWebView.h"

class GLGaeaClient;
class EngineDeviceMan;

namespace HTML5UI
{

class CGameRootWindow : public GUIBase::IWindow
{
public:
	CGameRootWindow( GUIBase::UIID uiID
		, const std::string& uiStrID
		, GUIBase::UIRECT rcParentGlobalPos
		, GUIBase::WINDOW_CFG windowCFG
		, GUIBase::IWebView* webView
		, GLGaeaClient* pGaeaClient
		, EngineDeviceMan* pEngineDevice );
	~CGameRootWindow();

public:
	/// ÃÊ±âÈ­ & ÆÄ±«;
	virtual void Initialize();
	virtual void Destroy();

	virtual bool OnKeyEvent( std::tr1::shared_ptr<GUIBase::CUIKeyEvent> keyEvent );

	/// Loop
	virtual bool FrameMove( float timeDelta );

	/// MsgProc
	virtual bool MsgProc( const GUIBase::CALLBACK_MESSAGE msg ) { return true; }

private:
	GLGaeaClient* m_pGaeaClient;
	EngineDeviceMan* m_pEngineDevice;
};

}