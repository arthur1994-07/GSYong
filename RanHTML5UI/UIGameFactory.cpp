#include "stdafx.h"
#include "UIGameFactory.h"
#include "GameRootWindow.h"
#include "InventoryWindow.h"
#include "WebPageWindow.h"
#include "UIWebView.h"

#include <GUIBase/UIImageData.h>
#include "../EngineLib/DeviceMan.h"
#include "../RanLogicClient/GLGaeaClient.h"

namespace HTML5UI
{

CUIGameFactory::CUIGameFactory(  GLGaeaClient* pGaeaClient
							   , EngineDeviceMan* pEngineDevice )
: m_pGaeaClient( pGaeaClient )
, m_pEngineDevice( pEngineDevice ) { }

CUIGameFactory::~CUIGameFactory() { }

GUIBase::IUIComponent* CUIGameFactory::CreateWindowForm(
	const std::string& strID,
	GUIBase::WINDOW_CFG& windowCFG,
	GUIBase::UIRECT rcGlobalPos )
{
	/// Window
	GUIBase::IWindow* pWindow = NULL;
	if( strID == "ROOT_UI" )
	{
		pWindow = new CGameRootWindow(
			ROOT_UI,
			strID,
			rcGlobalPos,
			windowCFG,
			NULL,
			m_pGaeaClient,
			m_pEngineDevice );
	}
	else if( strID == "INVENTORY" )
	{
		pWindow = new CInventoryWindow(
			INVENTORY,
			strID,
			rcGlobalPos,
			windowCFG,
			new CUIWebView( INVENTORY ),
			m_pGaeaClient,
			m_pEngineDevice );
	}
	else if( strID == "WEBPAGE" )
	{
		pWindow = new CWebPageWindow(
			WEBPAGE,
			strID,
			rcGlobalPos,
			windowCFG,
			new CUIWebView( WEBPAGE ),
			m_pGaeaClient,
			m_pEngineDevice );
	}
	pWindow->UIInitialize();

	return pWindow;
}

}