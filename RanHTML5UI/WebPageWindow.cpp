#include "StdAfx.h"
#include "WebPageWindow.h"

#include <GUIBase/UIManager.h>
#include <GUIBase/UIEventManager.h>
#include <GUIBase/UIWindowManager.h>

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/DeviceMan.h"
#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogicClient/Char/GLCharacter.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanUI/OldUI/Interface/SkillFunc.h"

namespace HTML5UI
{
	CWebPageWindow::CWebPageWindow(  GUIBase::UIID uiID
		, const std::string& uiStrID
		, GUIBase::UIRECT rcParentGlobalPos
		, GUIBase::WINDOW_CFG windowCFG
		, GUIBase::IWebView* webView
		, GLGaeaClient* pGaeaClient
		, EngineDeviceMan* pEngineDevice )
		: GUIBase::IWindow( uiID, uiStrID, rcParentGlobalPos, windowCFG, webView )
		, m_pGaeaClient( pGaeaClient )
		, m_pEngineDevice( pEngineDevice ) { }

	CWebPageWindow::~CWebPageWindow() { }

	/// 초기화 & 파괴;
	void CWebPageWindow::Initialize() { ShowComponent(); }

	void CWebPageWindow::Destroy() { }

	bool CWebPageWindow::OnMouseEvent( std::tr1::shared_ptr<GUIBase::CUIMouseEvent> mouseEvent )
	{
		if( !GUIBase::IWindow::OnMouseEvent( mouseEvent ) )
			return false;

		return true;
	}

	/// Loop
	bool CWebPageWindow::FrameMove( float timeDelta )
	{
		return true;
	}

	/// MsgProc
	bool CWebPageWindow::MsgProc( const GUIBase::CALLBACK_MESSAGE msg )
	{
		if( msg.message == "LOG" )
		{
			m_pGaeaClient->PrintConsoleText( msg.text );
		}
		else if( msg.message == "INITIALIZE" )
		{
			Json::Value sendRoot;

			// 초기화;
			sendRoot["message"] = "INITIALIZE";
			sendRoot["title"] = ID2GAMEWORD( "INVENTORY_NAME_STATIC" );
			FireInvokeEvent( sendRoot );
		}
		return true;
	}

}