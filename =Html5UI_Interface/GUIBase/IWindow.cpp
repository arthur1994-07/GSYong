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
	// Window�� URL������ ���ٴ� ����;
	// WebView�� ���õ� �κ��� �׸� ������ ���ٴ� �ǹ��̴�;
	if( m_url == L"" )
		return true;

	// Update �Ǵ� Image������ ȹ���Ѵ�;
	while( LoadImgUpdateData() )
	{
		// Update �� Image������ �����Ѵٸ� Texture�� �����Ѵ�;
		if( !CUIRenderManager::GetInstance().RenderTextureUsingData(
			m_uiID, m_uiImgData, m_position ) )
			return false;
	}

	// Window�� �����Ǵ� Texture�� ȭ�鿡 �׸���;
	CUIRenderManager::GetInstance().RenderUI(
		m_position.rcGlobalPos, m_uiID );

	return true;
}

/// �⺻������ �̷������ �ϴ� �κ�;
void IWindow::UIInitialize()
{
	if( m_webView.get())
	{
		m_webView->Create( this );						// WebView ����;
		m_webView->SetFrame( m_position.rcGlobalPos );	// ��ġ, ũ�� ����;
		m_webView->ChangeUI( m_url );					// URL ����;
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

	// Event ó��;
	EventProc();

	// Callback Message ó��;
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
// Window�� WebView�� �̿��Ͽ� �׸���;
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

/// Event ó��;
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
				// ������Ʈ�� �ɸ��� return;
				// WebView�� �������� �ʴ´�;
				// ������Ʈ ���� Event�� App��ü���� ó���Ѵ�;
				if( tempComponent->OnKeyEvent( keyEvent ) )
					return true;
			}
		}

		// WebView�� ����;
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
				// ������Ʈ�� �ɸ��� return;
				// WebView�� �������� �ʴ´�;
				// ������Ʈ ���� Event�� App��ü���� ó���Ѵ�;
				if( tempComponent->OnMouseEvent( mouseEvent ) )
					return true;
			}
		}

		// WebView�� ����;
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
				// ������Ʈ�� �ɸ��� return;
				// WebView�� �������� �ʴ´�;
				// ������Ʈ ���� Event�� App��ü���� ó���Ѵ�;
				if( tempComponent->OnWheelEvent( wheelEvent ) )
					return true;
			}
		}

		// WebView�� ����;
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

	// Invoke �߻� �� Invokes�� ����;
	if( m_webView.get() )
		m_invokeEvents.push_back( invokeEvent );

	// Child Component
	LIST_ITER iter = m_list.begin();
	LIST_ITER iter_end = m_list.end();
	for( ; iter!=iter_end; ++iter )
	{
		IUIComponent* tempComponent = *iter;

		// ������ ��� �ڽĿ� ����;
		// Invoke Event�� �׻� Update�Ǿ�߸� �ϴ� �����̹Ƿ�;
		// ��� �ڽĿ��� �����Ѵ�;
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
	// ���� ��Ŀ�� �����쿴�ٸ�;
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

/// Event �߻� �Լ�;
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

	// ���콺�� ���Դ��� Ȯ��;
	if( !CEventUtil::MouseIn(
		mouseEvent->GetGlobalPosition(),
		m_position.rcGlobalPos ) )
		return false;

	// ���콺�� LocalPos ����;
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
				// ���콺 �ٿ� �޽����� ���� ��Ŀ�� ����;
				if( mouseEvent->GetEventType() == CUIEvent::UI_MouseDown )
					tempWindow->ShowComponent();
				return true;
			}
		}
	}

	// ���;	
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

	// ���콺�� ���Դ��� Ȯ��;
	if( !CEventUtil::MouseIn(
		wheelEvent->GetGlobalPosition(),
		m_position.rcGlobalPos ) )
		return false;

	// ���콺�� LocalPos ����;
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

	// ���;
	std::tr1::shared_ptr< CUIEvent > sharedUIEvent( wheelEvent );
	m_pEventQueue.push( sharedUIEvent );

	return true;
}

bool IWindow::FireKeyEvent( CUIKeyEvent* keyEvent )
{
	if( !m_bShow || m_bBlock )
		return false;

	// ���;
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

		// HOLD_TITLEBAR Message�� ����´ٸ� Hold Window�� ����Ѵ�;
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

// �ڽ� ������ �߰�;
bool IWindow::AddChildWindowID( UIID uiID )
{
	// ���� ��Ŀ�� ������ ã��;
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

// �ڽ� ������ ��ȯ;
UIID IWindow::GetChildWindowID()
{
	// ���� ��Ŀ�� ������ ã��;
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

	// ã�� ���� ��� �ڱ��ڽ� ��ȯ;
	return m_uiID;
}

// �������� ����;
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

// Event ���;
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