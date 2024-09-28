#include "StdAfx.h"
#include "IWindow.h"
#include "IWebView.h"
#include "UIManager.h"
#include "UIRenderManager.h"
#include "UIWindowManager.h"
#include "UIEventManager.h"

namespace GUIBase
{

IWindow::IWindow(  UIID uiID
				   , const std::string& uiStrID
				   , UIRECT rcParentGlobalPos
				   , WINDOW_CFG windowCFG
				   , IWebView* webView )
				   : IUIComponent( uiID
				   , uiStrID
				   , rcParentGlobalPos
				   , windowCFG.rcWindowPos
				   , windowCFG.pParentWindow )
				   , m_url( windowCFG.url )
				   , m_webView( webView )
				   , m_bBlock( false )
{
	ZeroMemory( &m_uiImgData, sizeof( UI_IMAGE_DATA ) );
}

IWindow::~IWindow() { Destroy(); }

// Render Window;
bool IWindow::Render( float timeDelta )
{
	// Window에 URL정보가 없다는 것은;
	// WebView에 관련된 부분은 그릴 정보가 없다는 의미이다;
	if( m_url == L"" )
		return true;

	// Update 되는 Image정보를 획득한다;
	while( LoadImgUpdateData() )
	{
		// Update 된 Image정보가 존재한다면 Texture를 갱신한다;
		if( !CUIRenderManager::GetInstance().RenderTextureUsingData(
			m_uiID, m_uiImgData, m_position ) )
			return false;
	}

	// Window에 대응되는 Texture를 화면에 그린다;
	CUIRenderManager::GetInstance().RenderUI(
		m_position.rcGlobalPos, m_uiID );

	return true;
}

/// 기본적으로 이루어져야 하는 부분;
void IWindow::UIInitialize()
{
	if( m_webView.get())
	{
		m_webView->Create( this );						// WebView 생성;
		m_webView->SetFrame( m_position.rcGlobalPos );	// 위치, 크기 세팅;
		m_webView->ChangeUI( m_url );					// URL 연결;
		//HideComponent();
	}

	IUIComponent::UIInitialize();
}

void IWindow::UIDestroy()
{
	if( m_webView.get() )
	{
		m_webView->OnDestroy();

		IUIComponent::UIDestroy();
	}
}

bool IWindow::UIFrameMove( float timeDelta )
{
	if( !IUIComponent::UIFrameMove( timeDelta ) )
		return false;

	// Event 처리;
	EventProc();

	// Callback Message 처리;
	CALLBACK_MESSAGE callbackMsg;
	while( !m_msgQueue.empty() )
	{
		callbackMsg = m_msgQueue.front();
		m_msgQueue.pop();
		MsgProc( callbackMsg );
	}

	// Child Window
	WINDOW_LIST_ITER iter = m_windowLists.begin();
	WINDOW_LIST_ITER iter_end = m_windowLists.end();
	for( ; iter!=iter_end; ++iter )
	{
		IWindow* tempWindow = CUIWindowManager::GetInstance()
			.GetWindow( *iter );
		if( tempWindow )
		{
			if( !tempWindow->UIFrameMove( timeDelta ) )
				return false;
		}
	}

	return true;
}

//! Rendering Html Page
// Window는 WebView를 이용하여 그린다;
bool IWindow::UIRender( float timeDelta )
{
	if( !IUIComponent::UIRender( timeDelta ) )
		return false;

	// Child Window
	WINDOW_LIST_ITER iter = m_windowLists.begin();
	WINDOW_LIST_ITER iter_end = m_windowLists.end();
	for( ; iter!=iter_end; ++iter )
	{
		IWindow* tempWindow = CUIWindowManager::GetInstance()
			.GetWindow( *iter );
		if( tempWindow )
		{
			if( !tempWindow->UIRender( timeDelta ) )
				return false;
		}
	}

	return true;
}

/// Event 처리;
void IWindow::OnSize( std::tr1::shared_ptr<CUIResizeEvent> resizeEvent )
{
	if( resizeEvent.get() )
	{
		if( m_webView.get() )
			m_webView->OnSize( resizeEvent );
	}
}

void IWindow::OnFocus( std::tr1::shared_ptr<CUIFocusEvent> focusEvent )
{
	if( focusEvent.get() )
	{
		if( m_webView.get() )
			m_webView->OnFocus( focusEvent );
	}
}

void IWindow::OnShow( std::tr1::shared_ptr<CUIShowEvent> showEvent )
{
	if( showEvent.get() )
	{
		if( m_webView.get() )
			m_webView->OnShow( showEvent );

		m_bShow = true;
	}
}

void IWindow::OnHide( std::tr1::shared_ptr<CUIShowEvent> hideEvent )
{
	if( hideEvent.get() )
	{
		if( m_webView.get() )
			m_webView->OnHide( hideEvent );
		
		m_bShow = false;
	}
}

bool IWindow::OnKeyEvent( std::tr1::shared_ptr<CUIKeyEvent> keyEvent )
{
	if( !keyEvent.get() )
		return false;

	if( m_bShow && !m_bBlock )
	{
		// Child Component
		LIST_ITER iter = m_list.begin();
		LIST_ITER iter_end = m_list.end();
		for( ; iter!=iter_end; ++iter )
		{
			IUIComponent* tempComponent = *iter;
			if( tempComponent )
			{
				// 컴포넌트에 걸리면 return;
				// WebView에 전달하지 않는다;
				// 컴포넌트 관련 Event는 App자체에서 처리한다;
				if( tempComponent->OnKeyEvent( keyEvent ) )
					return true;
			}
		}

		// WebView에 전달;
		if( m_webView.get() )
			m_webView->OnKeyEvent( keyEvent );
		return true;
	}
	return false;
}

bool IWindow::OnMouseEvent( std::tr1::shared_ptr<CUIMouseEvent> mouseEvent )
{
	if( !mouseEvent.get() )
		return false;

	if( m_bShow && !m_bBlock )
	{
		// Child Component
		LIST_ITER iter = m_list.begin();
		LIST_ITER iter_end = m_list.end();
		for( ; iter!=iter_end; ++iter )
		{
			IUIComponent* tempComponent = *iter;
			if( tempComponent )
			{
				// 컴포넌트에 걸리면 return;
				// WebView에 전달하지 않는다;
				// 컴포넌트 관련 Event는 App자체에서 처리한다;
				if( tempComponent->OnMouseEvent( mouseEvent ) )
					return true;
			}
		}

		// WebView에 전달;
		if( m_webView.get() )
			m_webView->OnMouseEvent( mouseEvent );
		return true;
	}
	return false;
}

bool IWindow::OnWheelEvent( std::tr1::shared_ptr<CUIWheelEvent> wheelEvent )
{
	if( !wheelEvent.get() )
		return false;

	if( m_bShow && !m_bBlock )
	{
		// Child Component
		LIST_ITER iter = m_list.begin();
		LIST_ITER iter_end = m_list.end();
		for( ; iter!=iter_end; ++iter )
		{
			IUIComponent* tempComponent = *iter;
			if( tempComponent )
			{
				// 컴포넌트에 걸리면 return;
				// WebView에 전달하지 않는다;
				// 컴포넌트 관련 Event는 App자체에서 처리한다;
				if( tempComponent->OnWheelEvent( wheelEvent ) )
					return true;
			}
		}

		// WebView에 전달;
		if( m_webView.get() )
			m_webView->OnWheelEvent( wheelEvent );
		return true;
	}
	return false;
}

bool IWindow::OnInvokeEvent( std::tr1::shared_ptr<CUIInvokeEvent> invokeEvent )
{
	if( !invokeEvent.get() )
		return false;

	// Invoke 발생 시 Invokes에 저장;
	if( m_webView.get() )
		m_invokeEvents.push_back( invokeEvent );

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;

		// 무조건 모든 자식에 전달;
		// Invoke Event는 항상 Update되어야만 하는 정보이므로;
		// 모든 자식에게 전달한다;
		if( tempComponent )
			tempComponent->OnInvokeEvent( invokeEvent );
	}
	
	return true;
}

/// Show
void IWindow::ShowComponent()
{
	IWindow* pParent = static_cast<IWindow*>( m_pMyParent );
	if( pParent )
	{
		pParent->AddChildWindowID( m_uiID );
		CUIManager::GetInstance().RegisterFocusWindow( m_uiID );
	}

	CUIEvent* uiEvent = new CUIShowEvent(
		m_uiID, CUIEvent::UI_ShowWindow, true );

	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( uiEvent );
	m_pEventQueue.push( sharedUIEvent );
}

void IWindow::HideComponent()
{
	// 만약 포커스 윈도우였다면;
	if( CUIManager::GetInstance().GetFocusWindow() == m_uiID )
	{
		IWindow* pParent = static_cast<IWindow*>( m_pMyParent );
		if( pParent )
		{
			UIID uiID = pParent->GetChildWindowID();
			if( uiID != NO_ID )
				CUIManager::GetInstance().RegisterFocusWindow( uiID );
		}
		else
			CUIManager::GetInstance().RegisterFocusWindow( NO_ID );
	}

	CUIEvent* uiEvent = new CUIShowEvent(
		m_uiID, CUIEvent::UI_HideWindow, false );

	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( uiEvent );
	m_pEventQueue.push( sharedUIEvent );
}

/// Event 발생 함수;
bool IWindow::FireMouseEvent( CUIMouseEvent* mouseEvent )
{
	if( !m_bShow || m_bBlock )
		return false;

	if( !mouseEvent )
		return false;

	switch( mouseEvent->GetEventType() )
	{
	case CUIEvent::UI_MouseDown:
	case CUIEvent::UI_MouseLeave:
	case CUIEvent::UI_MouseMove:
	case CUIEvent::UI_MouseUp:
		break;
	default:
		return false;
	}

	// 마우스가 들어왔는지 확인;
	if( !CEventUtil::MouseIn(
		mouseEvent->GetGlobalPosition(),
		m_position.rcGlobalPos ) )
		return false;

	// 마우스의 LocalPos 갱신;
	UIPOINT<int> localPoint = mouseEvent->GetGlobalPosition();
	localPoint.x -= static_cast<int>( m_position.rcGlobalPos.left );
	localPoint.y -= static_cast<int>( m_position.rcGlobalPos.top );
	mouseEvent->SetPosition( localPoint );
	mouseEvent->SetUIID( m_uiID );

	// Child Window
	WINDOW_LIST_RITER riter = m_windowLists.rbegin();
	WINDOW_LIST_RITER riter_end = m_windowLists.rend();
	for( ; riter!=riter_end; ++riter )
	{
		IWindow* tempWindow = CUIWindowManager::GetInstance()
			.GetWindow( *riter );
		if( tempWindow )
		{
			if( tempWindow->FireMouseEvent( mouseEvent ) )
			{	
				// 마우스 다운 메시지가 들어가면 포커스 변경;
				if( mouseEvent->GetEventType() == CUIEvent::UI_MouseDown )
					tempWindow->ShowComponent();
				return true;
			}
		}
	}

	// 등록;	
	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( mouseEvent );
	m_pEventQueue.push( sharedUIEvent );

	return true;
}

bool IWindow::FireWheelEvent( CUIWheelEvent* wheelEvent )
{
	if( !m_bShow || m_bBlock )
		return false;

	if( !wheelEvent )
		return false;

	switch( wheelEvent->GetEventType() )
	{
	case CUIEvent::UI_Wheel:
		break;
	default:
		return false;
	}

	// 마우스가 들어왔는지 확인;
	if( !CEventUtil::MouseIn(
		wheelEvent->GetGlobalPosition(),
		m_position.rcGlobalPos ) )
		return false;

	// 마우스의 LocalPos 갱신;
	UIPOINT<int> localPoint = wheelEvent->GetGlobalPosition();
	localPoint.x -= static_cast<int>( m_position.rcGlobalPos.left );
	localPoint.y -= static_cast<int>( m_position.rcGlobalPos.top );
	wheelEvent->SetPosition( localPoint );
	wheelEvent->SetUIID( m_uiID );

	// Child Window
	WINDOW_LIST_RITER iter = m_windowLists.rbegin();
	WINDOW_LIST_RITER iter_end = m_windowLists.rend();
	for( ; iter!=iter_end; ++iter )
	{
		IWindow* tempWindow = CUIWindowManager::GetInstance()
			.GetWindow( *iter );
		if( tempWindow )
		{
			if( tempWindow->FireWheelEvent( wheelEvent ) ) {
				tempWindow->ShowComponent();
				return true;
			}
		}
	}

	// 등록;
	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( wheelEvent );
	m_pEventQueue.push( sharedUIEvent );

	return true;
}

bool IWindow::FireKeyEvent( CUIKeyEvent* keyEvent )
{
	if( !m_bShow || m_bBlock )
		return false;

	// 등록;
	if( !keyEvent )
		return false;

	switch( keyEvent->GetEventType() )
	{
	case CUIEvent::UI_Char:
	case CUIEvent::UI_KeyDown:
	case CUIEvent::UI_KeyUp:
		break;
	default:
		return false;
	}

	keyEvent->SetUIID( m_uiID );

	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( keyEvent );
	m_pEventQueue.push( sharedUIEvent );

	return true;
}

bool IWindow::FireInvokeEvent( Json::Value& root )
{
	GUIBase::CUIInvokeEvent* invokeEvent = new GUIBase::CUIInvokeEvent(
		m_uiID,
		GUIBase::CUIEvent::UI_Invoke,
		ToUnicode( CUIJsonParser::Write( root ).c_str() ),
		0.0 );
	root.clear();

	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( invokeEvent );
	m_pEventQueue.push( sharedUIEvent );

	return true;
}

void IWindow::UIPageCallback( const std::string& message )
{
	static Json::Value root;
	if( CUIJsonParser::Read( message, root ) )
	{
		CALLBACK_MESSAGE callbackMsg;
		callbackMsg.message = root["message"].asString();
		callbackMsg.x = root["x"].asInt();
		callbackMsg.y = root["y"].asInt();
		callbackMsg.value = root["value"].asInt();
		callbackMsg.text = root["text"].asString();

		// HOLD_TITLEBAR Message가 날라온다면 Hold Window를 등록한다;
		if( callbackMsg.message == "HOLD_TITLEBAR" )
		{
			CUIManager::GetInstance().RegisterHoldWindow( m_uiID );
			m_mouseGap.x = callbackMsg.x;
			m_mouseGap.y = callbackMsg.y;
		}
		else
			m_msgQueue.push( callbackMsg );
	}
}

// Array Invoke;
void IWindow::FireArrayInvokeEvent()
{
	if( !m_invokeEvents.empty() )
	{
		std::tr1::shared_ptr<CUIArrayInvokeEvent> arrayInvoke(
			new CUIArrayInvokeEvent( m_uiID,
			CUIEvent::UI_Invoke,
			m_invokeEvents.size() ) );

		for( int i=0; i<m_invokeEvents.size(); ++i )
		{
			arrayInvoke->AddInvokeMessage(
				m_invokeEvents.at( i )->GetInvokeMessage(), i );
		}
		m_invokeEvents.clear();
		m_webView->OnInvokeEvent( arrayInvoke );
	}
}

// 자식 윈도우 추가;
bool IWindow::AddChildWindowID( UIID uiID )
{
	// 현재 포커스 윈도우 찾기;
	WINDOW_LIST_ITER iter = m_windowLists.begin();
	WINDOW_LIST_ITER iter_end = m_windowLists.end();
	for( ; iter!=iter_end; ++iter )
	{
		if( *iter == uiID )
		{
			m_windowLists.erase( iter );
			m_windowLists.push_back( uiID );
			return true;
		}
	}

	return false;
}

// 자식 윈도우 반환;
UIID IWindow::GetChildWindowID()
{
	// 현재 포커스 윈도우 찾기;
	WINDOW_LIST_RITER riter = m_windowLists.rbegin();
	WINDOW_LIST_RITER riter_end = m_windowLists.rend();
	for( ; riter!=riter_end; ++riter )
	{
		IWindow* tempWindow = CUIWindowManager::GetInstance()
			.GetWindow( *riter );
		if( tempWindow )
		{
			if( tempWindow->ShowState() )
				return tempWindow->GetUIID();
		}
	}

	// 찾지 못할 경우 자기자신 반환;
	return m_uiID;
}

// 웹페이지 갱신;
void IWindow::UpdateWebPage()
{
	if( m_webView.get() )
		m_webView->UpdateWebPage();

	// Child Window
	WINDOW_LIST_ITER iter = m_windowLists.begin();
	WINDOW_LIST_ITER iter_end = m_windowLists.end();
	for( ; iter!=iter_end; ++iter )
	{
		IWindow* tempWindow = CUIWindowManager::GetInstance()
			.GetWindow( *iter );
		if( tempWindow )
			tempWindow->UpdateWebPage();
	}
}

// Event 등록;
void IWindow::FireUIEvent( CUIEvent* uiEvent )
{
	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( uiEvent );
	m_pEventQueue.push( sharedUIEvent );
}

// Update Image Data
bool IWindow::LoadImgUpdateData()
{
	if( m_webView.get() )
		return m_webView->GetImgUpdateData( m_uiImgData );
	return false;
}

}