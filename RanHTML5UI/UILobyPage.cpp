#include "StdAfx.h"
#include "UILobyPage.h"
//#include "UIGameFactory.h"

#include <GUIBase/UIManager.h>
#include <GUIBase/UIWindowManager.h>
#include <GUIBase/UIEventManager.h>
#include <GUIBase/UIRenderManager.h>

#include "../EngineLib/DeviceMan.h"
#include "../RanLogicClient/GLGaeaClient.h"

namespace HTML5UI
{

CUILobyPage::CUILobyPage(  GLGaeaClient* pGaeaClient
						 , EngineDeviceMan* pEngineDevice )
						 : GUIBase::IPage()
						 , m_pGaeaClient( pGaeaClient )
						 , m_pEngineDevice( pEngineDevice ) { }

CUILobyPage::~CUILobyPage() { }

void CUILobyPage::Initialize()
{
	/*// WindowManager ( Stage별로 다른 Factory를 가진다 );
	GUIBase::CUIWindowManager::GetInstance().ChangeUIFactory(
		new CUILobyFactory( m_pGaeaClient, m_pEngineDevice ) );

	// GameRoot UI 추가;
	GUIBase::WINDOW_CFG rootCFG;
	rootCFG.rcWindowPos = GUIBase::UIRECT( 0, 0, 1024, 768 );
	GUIBase::CUIWindowManager::GetInstance().InsertWindow(
		"LOBY_UI", rootCFG );

	// RootUI 등록;
	m_pBasicWindow = GUIBase::CUIWindowManager::GetInstance()
		.GetWindow( "LOBY_UI" );*/
}

void CUILobyPage::Destroy() { }

/// Loop
bool CUILobyPage::FrameMove( float timeDelta )
{
	/// 삽입;
	/*int KEY = (int)DxInputDevice::GetInstance().GetKeyState(DIK_F6);
	if( ( KEY & DXKEY_DOWN ) || ( KEY & DXKEY_PRESSED ) )
	{
		GUIBase::CUIManager::GetInstance().ShowComponent(
			CGameUIFactory::TEST_UI_WINDOW );
	}*/

	return true;
}

bool CUILobyPage::Render( float timeDelta )
{
	return true;
}

}