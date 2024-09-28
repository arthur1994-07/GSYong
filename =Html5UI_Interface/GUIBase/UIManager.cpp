#include "StdAfx.h"
#include "UIManager.h"
#include "IPage.h"
#include "UIEventManager.h"
#include "UIRenderManager.h"
#include "UIWindowManager.h"

namespace GUIBase
{

CUIManager& CUIManager::GetInstance()
{
	static CUIManager instance;
	return instance;
}

/// Device 처리;
bool CUIManager::InitDeviceObjects()
{
	// Initialize Device
	if( !CUIRenderManager::GetInstance().InitDevice() )
		return false;
	
	return true;
}

bool CUIManager::InvalidateDeviceObjects()
{
	// Invalidate Device
	if( !CUIRenderManager::GetInstance().InvalidateDevice() )
		return false;

	return true;
}

bool CUIManager::RestoreDeviceObjects()
{
	// Restore Device
	if( !CUIRenderManager::GetInstance().RestoreDevice() )
		return false;

	m_pPage->UpdateWebPage();
	
	return true;
}

bool CUIManager::DeleteDeviceObjects()
{
	// Delete Device
	if( !CUIRenderManager::GetInstance().DeleteDevice() )
		return false;

	return true;
}

//! FrameMove
bool CUIManager::FrameMove( float timeDelta )
{
	if( m_pPage )
	{
		// FrameMove Root Window
		if( !m_pPage->UIFrameMove( timeDelta ) )
			return false;

		// Event 처리;
		CUIEventManager::GetInstance().EventUpdate();
	}

	return true;
}

//! Render
bool CUIManager::Render( float timeDelta )
{
	if( m_pPage )
	{
		if( !m_pPage->UIRender( timeDelta ) )
			return false;
	}

	return true;
}

// 포커스 등록;
void CUIManager::RegisterFocusWindow( UIID uiID )
{
	IWindow* pWindow = NULL;
	Json::Value sendRoot;
	if( m_focusUIID != NO_ID )
	{
		pWindow = CUIWindowManager::GetInstance()
			.GetWindow( m_focusUIID );

		if( pWindow )
		{
			sendRoot["message"] = "UNFOCUS_WINDOW";
			pWindow->FireInvokeEvent( sendRoot );
		}
	}

	if( uiID != NO_ID )
	{
		pWindow = CUIWindowManager::GetInstance()
			.GetWindow( uiID );

		if( pWindow )
		{
			sendRoot["message"] = "FOCUS_WINDOW";
			pWindow->FireInvokeEvent( sendRoot );
		}
	}

	m_focusUIID = uiID;
}

//! UI Stage 변경;
void CUIManager::ChangeUIStage( IPage* pPage )
{
	if( !pPage )
		return;

	SAFE_DELETE( m_pPage );

	m_pPage = pPage;
	m_pPage->UIInitialize();
}

//! Mouse, Keyboard, Wheel 등의 이벤트 전달;
bool CUIManager::FireMouseEvent( CUIMouseEvent* mouseEvent )
{
	if( !m_pPage )
		return false;

	// Drag 처리;
	if( mouseEvent->GetEventType() == CUIEvent::UI_MouseMove )
	{
		UIPOINT<int> globalPos = mouseEvent->GetGlobalPosition();
		IWindow* window = CUIWindowManager::GetInstance().GetWindow(
			CUIManager::GetInstance().GetHoldWindow() );

		if( window )
		{
			globalPos.x -= window->GetMouseGap().x;
			globalPos.y -= window->GetMouseGap().y;
			window->MoveComponentPoint( globalPos );
		}
	}

	// MouseUp Event가 들어오면 등록된 들린 마우스 해제;
	if( mouseEvent->GetEventType() == CUIEvent::UI_MouseUp &&
		m_holdUIID != -1 )
		m_holdUIID = -1;

	if( !m_pPage->FireMouseEvent( mouseEvent ) )
		return false;

	return true;
}

bool CUIManager::FireWheelEvent( CUIWheelEvent* wheelEvent )
{
	if( !m_pPage )
		return false;

	if( !m_pPage->FireWheelEvent( wheelEvent ) )
		return false;

	return true;
}

bool CUIManager::FireKeyEvent( CUIKeyEvent* keyEvent )
{
	if( !m_pPage )
		return false;

	if( !m_pPage->FireKeyEvent( keyEvent ) )
		return false;

	return true;
}

// WebPage Update;
void CUIManager::UpdateWebPage()
{
	if( m_pPage )
		m_pPage->UpdateWebPage();
}

CUIManager::CUIManager()
: m_pPage( NULL )
, m_holdUIID( -1 )
, m_focusUIID( -1 )
, m_keyFocusUIID( -1 ) { }

CUIManager::~CUIManager()
{
	SAFE_DELETE( m_pPage );
}

}