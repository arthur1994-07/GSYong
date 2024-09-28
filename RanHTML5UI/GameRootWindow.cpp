#include "StdAfx.h"
#include "GameRootWindow.h"

#include <GUIBase/UIManager.h>
#include <GUIBase/UIEventManager.h>
#include <GUIBase/UIWindowManager.h>

#include "../RanHTML5UI/UIGameFactory.h"

#include "../EngineLib/DeviceMan.h"
#include "../RanLogicClient/GLGaeaClient.h"

namespace HTML5UI
{

CGameRootWindow::CGameRootWindow(  GUIBase::UIID uiID
						 , const std::string& uiStrID
						 , GUIBase::UIRECT rcParentGlobalPos
						 , GUIBase::WINDOW_CFG windowCFG
						 , GUIBase::IWebView* webView
						 , GLGaeaClient* pGaeaClient
						 , EngineDeviceMan* pEngineDevice )
						 : GUIBase::IWindow( uiID, uiStrID, rcParentGlobalPos, windowCFG, webView )
						 , m_pGaeaClient( pGaeaClient )
						 , m_pEngineDevice( pEngineDevice ) { }

CGameRootWindow::~CGameRootWindow() { }

/// �ʱ�ȭ & �ı�;
void CGameRootWindow::Initialize()
{
	GUIBase::WINDOW_CFG rootCFG;
	GUIBase::UIID uiID = GUIBase::NO_ID;

	// �⺻ ��Ŀ�� ������ ���;
	GUIBase::CUIManager::GetInstance().SetKeyFocusWindow( m_uiID );

	// Show
	ShowComponent();

	// �θ������� ���;
	rootCFG.pParentWindow = this;

	// Inventory UI �߰�;
	rootCFG.rcWindowPos = GUIBase::UIRECT( 100, 100, 239, 510 );
	rootCFG.url = L"file:///D:/MyWorkspace/JSProject/workspace/RanUI/WebContent/InventoryWindow.html";
	GUIBase::CUIWindowManager::GetInstance().InsertWindow(
		"INVENTORY", rootCFG );

	// Inventory ���;
	uiID = GUIBase::CUIWindowManager::GetInstance()
		.LoadUIIDUsingStrID( "INVENTORY" );
	if( uiID != GUIBase::NO_ID )
		m_windowLists.push_back( uiID );

	// WebPage UI �߰�;
	rootCFG.rcWindowPos = GUIBase::UIRECT( 200, 200, 239, 400 );
	rootCFG.url = L"http://localhost:8080/cocos2d-html5/index.html";//L"file:///D:/MyWorkspace/JSProject/workspace/RanUI/WebContent/WebPageWindow.html";
	GUIBase::CUIWindowManager::GetInstance().InsertWindow(
		"WEBPAGE", rootCFG );

	// Inventory ���;
	uiID = GUIBase::CUIWindowManager::GetInstance()
		.LoadUIIDUsingStrID( "WEBPAGE" );
	if( uiID != GUIBase::NO_ID )
		m_windowLists.push_back( uiID );
}

void CGameRootWindow::Destroy() { }

bool CGameRootWindow::OnKeyEvent( std::tr1::shared_ptr<GUIBase::CUIKeyEvent> keyEvent )
{
	/// ���������δ� Ű���ÿ� �ִ� Ű �̺�Ʈ���� ��� Root�� ����;
	/// �Ͼ�� �Ѵ�.;
	if( keyEvent )
	{
		GUIBase::IWindow* pWindow = NULL;
		if( keyEvent->key() == MapVirtualKey( DIK_I, 1 ) )
		{
			pWindow = GUIBase::CUIWindowManager::GetInstance()
				.GetWindow( CUIGameFactory::INVENTORY );
		}
		else if( keyEvent->key() == MapVirtualKey( DIK_W, 1 ) )
		{
			pWindow = GUIBase::CUIWindowManager::GetInstance()
				.GetWindow( CUIGameFactory::WEBPAGE );
		}

		if( pWindow )
		{
			if( pWindow->ShowState() )
				pWindow->HideComponent();
			else
				pWindow->ShowComponent();
		}
	}

	return true;
}

/// Loop
bool CGameRootWindow::FrameMove( float timeDelta )
{
	return true;
}

}