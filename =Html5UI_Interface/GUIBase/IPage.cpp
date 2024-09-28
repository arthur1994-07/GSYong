#include "StdAfx.h"
#include "IPage.h"
#include "IWindow.h"
#include "UIManager.h"
#include "UIEventManager.h"
#include "UIWindowManager.h"

namespace GUIBase
{

IPage::IPage() : m_pBasicWindow( NULL )
{
}

void IPage::UIInitialize()
{
	Initialize();
}

void IPage::UIDestroy()
{
	Destroy();
}

bool IPage::UIFrameMove( float timeDelta )
{
	if( !FrameMove( timeDelta ) )
		return false;

	if( m_pBasicWindow )
		return m_pBasicWindow->UIFrameMove( timeDelta );

	return true;
}

bool IPage::UIRender( float timeDelta )
{
	if( !Render( timeDelta ) )
		return false;

	if( m_pBasicWindow )
		return m_pBasicWindow->UIRender( timeDelta );

	return true;
}

//! Mouse, Keyboard, Wheel 등의 이벤트 전달;
bool IPage::FireMouseEvent( CUIMouseEvent* mouseEvent )
{
	if( m_pBasicWindow )
		return m_pBasicWindow->FireMouseEvent( mouseEvent );

	return true;
}

bool IPage::FireWheelEvent( CUIWheelEvent* wheelEvent )
{
	if( m_pBasicWindow )
		return m_pBasicWindow->FireWheelEvent( wheelEvent );

	return true;
}

bool IPage::FireKeyEvent( CUIKeyEvent* keyEvent )
{
	if( !m_pBasicWindow )
		return true;

	UIID uiID = CUIManager::GetInstance().GetKeyFocusWindow();
	if( uiID == -1 || uiID == NO_ID  )
		return true;

	IWindow* pWindow = CUIWindowManager::GetInstance()
		.GetWindow( uiID );

	if( pWindow )
	{
		if( !pWindow->FireKeyEvent( keyEvent ) )
			return false;
	}

	return true;
}

void IPage::UpdateWebPage()
{
	if( m_pBasicWindow )
		m_pBasicWindow->UpdateWebPage();
}

}