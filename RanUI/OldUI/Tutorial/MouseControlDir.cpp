#include "StdAfx.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"

#include "MouseTip.h"
#include "../../../EngineLib/DxTools/TextureManager.h"
#include "../../../EngineLib/DxLand/DxLandMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/DxTools/DxRenderStates.h"
#include "../../../EngineLib/DxTools/DxViewPort.h"
#include "../../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../InnerInterface.h"
#include "MouseControlDir.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTutorialInterfaceLeft::CTutorialInterfaceLeft(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pEmptyLeft(NULL)
    , m_pFullLeft(NULL)
    , m_fFlowTime(0.0f)
    , m_pDoneLeft(NULL)
{
}

CTutorialInterfaceLeft::~CTutorialInterfaceLeft(void)
{
}

void CTutorialInterfaceLeft::CreateSubControl ()
{
	{
		CUIControl* pEmptyLeft = new CUIControl(m_pEngineDevice);
		pEmptyLeft->CreateSub ( this, "TUTORIAL_INTERFACE_EMPTY_LEFT" );
		RegisterControl ( pEmptyLeft );
		m_pEmptyLeft = pEmptyLeft;
		m_pEmptyLeft->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pFullLeft = new CUIControl(m_pEngineDevice);
		pFullLeft->CreateSub ( this, "TUTORIAL_INTERFACE_FULL_LEFT" );
		RegisterControl ( pFullLeft );
		m_pFullLeft = pFullLeft;
		m_pFullLeft->SetVisibleSingle( FALSE );
	}
	{
		CUIControl* pDoneLeft = new CUIControl(m_pEngineDevice);
		pDoneLeft->CreateSub ( this, "TUTORIAL_INTERFACE_DONE_LEFT" );
		RegisterControl ( pDoneLeft );
		m_pDoneLeft = pDoneLeft;
		m_pDoneLeft->SetVisibleSingle( FALSE );
	}

	m_pFullLeft->SetFlowBar( m_pFullLeft->GetGlobalPos(), m_pFullLeft->GetTexturePos() );
	m_pFullLeft->SetFlowBarType( CUIControl::FLOW_BAR_LEFT );
}

void CTutorialInterfaceLeft::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pFullLeft->IsVisible() )
	{
		float deltaY = -m_pGaeaClient->GetTutorial()->m_vCameraRotate.y / 1.6f;
		m_fFlowTime += deltaY;

		if ( m_fFlowTime >= 1.0f )
			m_fFlowTime = 1.0f;
		else if ( m_fFlowTime <= 0.0f )
			m_fFlowTime = 0.0f;

		if ( m_fFlowTime >= 1.0f )
		{
			m_pGaeaClient->GetTutorial()->NextSubStep();

			m_pFullLeft->SetVisibleSingle( FALSE );
			m_pDoneLeft->SetVisibleSingle( TRUE );
		}
		else
			m_pFullLeft->FlowBarUpdate( m_fFlowTime );
	}
}

HRESULT CTutorialInterfaceLeft::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcWindow = GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES / 3.0f - rcWindow.sizeX;
		vPos.y = Y_RES * 0.5f - rcWindow.sizeY * 0.5f;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	return S_OK;
}

void CTutorialInterfaceLeft::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CTutorialInterfaceLeft::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pFullLeft->SetVisibleSingle( TRUE );
}

CTutorialInterfaceRight::CTutorialInterfaceRight(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pEmptyRight(NULL)
    , m_pFullRight(NULL)
    , m_fFlowTime(0.0f)
    , m_pDoneRight(NULL)
{
}

CTutorialInterfaceRight::~CTutorialInterfaceRight(void)
{
}

void CTutorialInterfaceRight::CreateSubControl ()
{
	{
		CUIControl* pEmptyRight = new CUIControl(m_pEngineDevice);
		pEmptyRight->CreateSub ( this, "TUTORIAL_INTERFACE_EMPTY_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pEmptyRight );
		m_pEmptyRight = pEmptyRight;
		m_pEmptyRight->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pFullRight = new CUIControl(m_pEngineDevice);
		pFullRight->CreateSub ( this, "TUTORIAL_INTERFACE_FULL_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pFullRight );
		m_pFullRight = pFullRight;
		m_pFullRight->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pDoneRight = new CUIControl(m_pEngineDevice);
		pDoneRight->CreateSub ( this, "TUTORIAL_INTERFACE_DONE_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pDoneRight );
		m_pDoneRight = pDoneRight;
		m_pDoneRight->SetVisibleSingle( FALSE );
	}

	m_pFullRight->SetFlowBar( m_pFullRight->GetGlobalPos(), m_pFullRight->GetTexturePos() );
	m_pFullRight->SetFlowBarType( CUIControl::FLOW_BAR_RIGHT );
}

void CTutorialInterfaceRight::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pFullRight->IsVisible() )
	{
		float deltaY = m_pGaeaClient->GetTutorial()->m_vCameraRotate.y / 1.6f;
		m_fFlowTime += deltaY;

		if ( m_fFlowTime >= 1.0f )
			m_fFlowTime = 1.0f;
		else if ( m_fFlowTime <= 0.0f )
			m_fFlowTime = 0.0f;

		if ( m_fFlowTime >= 1.0f )
		{
			m_pGaeaClient->GetTutorial()->NextSubStep();

			m_pFullRight->SetVisibleSingle( FALSE );
			m_pDoneRight->SetVisibleSingle( TRUE );
		}
		else
			m_pFullRight->FlowBarUpdate( m_fFlowTime );
	}
}

HRESULT CTutorialInterfaceRight::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcWindow = GetGlobalPos ();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES  * 2.0f / 3.0f;
		vPos.y = Y_RES * 0.5f - rcWindow.sizeY * 0.5f;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	return S_OK;
}

void CTutorialInterfaceRight::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CTutorialInterfaceRight::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pFullRight->SetVisibleSingle( TRUE );
}

CTutorialInterfaceUp::CTutorialInterfaceUp(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pEmptyUp(NULL)
    , m_pFullUp(NULL)
    , m_fFlowTime(0.0f)
    , m_pDoneUp(NULL)
{
}

CTutorialInterfaceUp::~CTutorialInterfaceUp(void)
{
}

void CTutorialInterfaceUp::CreateSubControl ()
{
	{
		CUIControl* pEmptyUp = new CUIControl(m_pEngineDevice);
		pEmptyUp->CreateSub ( this, "TUTORIAL_INTERFACE_EMPTY_UP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pEmptyUp );
		m_pEmptyUp = pEmptyUp;
		m_pEmptyUp->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pFullUp= new CUIControl(m_pEngineDevice);
		pFullUp->CreateSub ( this, "TUTORIAL_INTERFACE_FULL_UP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pFullUp );
		m_pFullUp= pFullUp;
		m_pFullUp->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pDoneUp= new CUIControl(m_pEngineDevice);
		pDoneUp->CreateSub ( this, "TUTORIAL_INTERFACE_DONE_UP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pDoneUp );
		m_pDoneUp= pDoneUp;
		m_pDoneUp->SetVisibleSingle( FALSE );
	}

	m_pFullUp->SetFlowBar( m_pFullUp->GetGlobalPos(), m_pFullUp->GetTexturePos() );
	m_pFullUp->SetFlowBarType( CUIControl::FLOW_BAR_UP );
}

void CTutorialInterfaceUp::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pFullUp->IsVisible() )
	{
		float deltaX = -m_pGaeaClient->GetTutorial()->m_vCameraRotate.x / 0.6f;
		m_fFlowTime += deltaX;

		if ( m_fFlowTime >= 1.0f )
			m_fFlowTime = 1.0f;
		else if ( m_fFlowTime <= 0.0f )
			m_fFlowTime = 0.0f;

		if ( m_fFlowTime >= 1.0f )
		{
			m_pGaeaClient->GetTutorial()->NextSubStep();

			m_pFullUp->SetVisibleSingle( FALSE );
			m_pDoneUp->SetVisibleSingle( TRUE );
		}
		else
			m_pFullUp->FlowBarUpdate( m_fFlowTime );
	}
}

HRESULT CTutorialInterfaceUp::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcWindow = m_pEmptyUp->GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = Y_RES / 3.0f - rcWindow.sizeY;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	return S_OK;
}

void CTutorialInterfaceUp::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CTutorialInterfaceUp::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pFullUp->SetVisibleSingle( TRUE );
}

CTutorialInterfaceDown::CTutorialInterfaceDown(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pEmptyDown(NULL)
    , m_pFullDown(NULL)
    , m_fFlowTime(0.0f)
    , m_pDoneDown(NULL)
{
}

CTutorialInterfaceDown::~CTutorialInterfaceDown(void)
{
}

void CTutorialInterfaceDown::CreateSubControl ()
{
	{
		CUIControl* pEmptyDown = new CUIControl(m_pEngineDevice);
		pEmptyDown->CreateSub ( this, "TUTORIAL_INTERFACE_EMPTY_DOWN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pEmptyDown );
		m_pEmptyDown = pEmptyDown;
		m_pEmptyDown->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pFullDown= new CUIControl(m_pEngineDevice);
		pFullDown->CreateSub ( this, "TUTORIAL_INTERFACE_FULL_DOWN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pFullDown );
		m_pFullDown= pFullDown;
		m_pFullDown->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pDoneDown= new CUIControl(m_pEngineDevice);
		pDoneDown->CreateSub ( this, "TUTORIAL_INTERFACE_DONE_DOWN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pDoneDown );
		m_pDoneDown= pDoneDown;
		m_pDoneDown->SetVisibleSingle( FALSE );
	}

	m_pFullDown->SetFlowBar( m_pFullDown->GetGlobalPos(), m_pFullDown->GetTexturePos() );
	m_pFullDown->SetFlowBarType( CUIControl::FLOW_BAR_DOWN );
}

void CTutorialInterfaceDown::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pFullDown->IsVisible() )
	{
		float deltaX = m_pGaeaClient->GetTutorial()->m_vCameraRotate.x / 0.6f;
		m_fFlowTime += deltaX;

		if ( m_fFlowTime >= 1.0f )
			m_fFlowTime = 1.0f;
		else if ( m_fFlowTime <= 0.0f )
			m_fFlowTime = 0.0f;

		if ( m_fFlowTime >= 1.0f )
		{
			m_pGaeaClient->GetTutorial()->NextSubStep();

			m_pFullDown->SetVisibleSingle( FALSE );
			m_pDoneDown->SetVisibleSingle( TRUE );
		}
		else
			m_pFullDown->FlowBarUpdate( m_fFlowTime );
	}
}

HRESULT CTutorialInterfaceDown::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcWindow = m_pEmptyDown->GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = Y_RES * 2.0f / 3.0f;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	return S_OK;
}

void CTutorialInterfaceDown::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CTutorialInterfaceDown::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pFullDown->SetVisibleSingle( TRUE );
}

CTutorialInterfaceZoomIn::CTutorialInterfaceZoomIn(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pEmptyZoomIn(NULL)
    , m_pFullZoomIn(NULL)
    , m_fFlowTime(0.0f)
    , m_pDoneZoomIn(NULL)
{
}

CTutorialInterfaceZoomIn::~CTutorialInterfaceZoomIn(void)
{
}

void CTutorialInterfaceZoomIn::CreateSubControl ()
{
	{
		CUIControl* pEmptyZoomIn = new CUIControl(m_pEngineDevice);
		pEmptyZoomIn->CreateSub ( this, "TUTORIAL_INTERFACE_EMPTY_ZOOM_IN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pEmptyZoomIn );
		m_pEmptyZoomIn = pEmptyZoomIn;
		m_pEmptyZoomIn->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pFullZoomIn= new CUIControl(m_pEngineDevice);
		pFullZoomIn->CreateSub ( this, "TUTORIAL_INTERFACE_FULL_ZOOM_IN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pFullZoomIn );
		m_pFullZoomIn = pFullZoomIn;
		m_pFullZoomIn->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pDoneZoomIn= new CUIControl(m_pEngineDevice);
		pDoneZoomIn->CreateSub ( this, "TUTORIAL_INTERFACE_DONE_ZOOM_IN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pDoneZoomIn );
		m_pDoneZoomIn = pDoneZoomIn;
		m_pDoneZoomIn->SetVisibleSingle( FALSE );
	}

	m_pFullZoomIn->SetFlowBar( m_pFullZoomIn->GetGlobalPos(), m_pFullZoomIn->GetTexturePos() );
	m_pFullZoomIn->SetFlowBarType( CUIControl::FLOW_BAR_DOWN );
}

void CTutorialInterfaceZoomIn::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pFullZoomIn->IsVisible() )
	{
		float delta = m_pGaeaClient->GetTutorial()->m_fCameraZoom / 80.0f;
		m_fFlowTime += delta;

		if ( m_fFlowTime >= 1.0f )
			m_fFlowTime = 1.0f;
		else if ( m_fFlowTime <= 0.0f )
			m_fFlowTime = 0.0f;

		if ( m_fFlowTime >= 1.0f )
		{
			m_pGaeaClient->GetTutorial()->NextSubStep();

			m_pFullZoomIn->SetVisibleSingle( FALSE );
			m_pDoneZoomIn->SetVisibleSingle( TRUE );
		}
		else
			m_pFullZoomIn->FlowBarUpdate( m_fFlowTime );
	}
}

HRESULT CTutorialInterfaceZoomIn::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcWindow = m_pEmptyZoomIn->GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = Y_RES * 5.0f / 6.0f;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	return S_OK;
}

void CTutorialInterfaceZoomIn::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CTutorialInterfaceZoomIn::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pFullZoomIn->SetVisibleSingle( TRUE );
}

CTutorialInterfaceZoomOut::CTutorialInterfaceZoomOut(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pEmptyZoomOut(NULL)
    , m_pFullZoomOut(NULL)
    , m_fFlowTime(0.0f)
    , m_pDoneZoomOut(NULL)
{
}

CTutorialInterfaceZoomOut::~CTutorialInterfaceZoomOut(void)
{
}

void CTutorialInterfaceZoomOut::CreateSubControl ()
{
	{
		CUIControl* pEmptyZoomOut = new CUIControl(m_pEngineDevice);
		pEmptyZoomOut->CreateSub ( this, "TUTORIAL_INTERFACE_EMPTY_ZOOM_OUT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pEmptyZoomOut );
		m_pEmptyZoomOut = pEmptyZoomOut;
		m_pEmptyZoomOut->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pFullZoomOut= new CUIControl(m_pEngineDevice);
		pFullZoomOut->CreateSub ( this, "TUTORIAL_INTERFACE_FULL_ZOOM_OUT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pFullZoomOut );
		m_pFullZoomOut = pFullZoomOut;
		m_pFullZoomOut->SetVisibleSingle( FALSE );
	}

	{
		CUIControl* pDoneZoomOut= new CUIControl(m_pEngineDevice);
		pDoneZoomOut->CreateSub ( this, "TUTORIAL_INTERFACE_DONE_ZOOM_OUT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pDoneZoomOut );
		m_pDoneZoomOut = pDoneZoomOut;
		m_pDoneZoomOut->SetVisibleSingle( FALSE );
	}

	m_pFullZoomOut->SetFlowBar( m_pFullZoomOut->GetGlobalPos(), m_pFullZoomOut->GetTexturePos() );
	m_pFullZoomOut->SetFlowBarType( CUIControl::FLOW_BAR_UP );
}

void CTutorialInterfaceZoomOut::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( m_pFullZoomOut->IsVisible() )
	{
		float delta = -m_pGaeaClient->GetTutorial()->m_fCameraZoom / 80.0f;
		m_fFlowTime += delta;

		if ( m_fFlowTime >= 1.0f )
			m_fFlowTime = 1.0f;
		else if ( m_fFlowTime <= 0.0f )
			m_fFlowTime = 0.0f;

		if ( m_fFlowTime >= 1.0f )
		{
			m_pGaeaClient->GetTutorial()->NextSubStep();

			m_pFullZoomOut->SetVisibleSingle( FALSE );
			m_pDoneZoomOut->SetVisibleSingle( TRUE );
		}
		else
			m_pFullZoomOut->FlowBarUpdate( m_fFlowTime );
	}
}

HRESULT CTutorialInterfaceZoomOut::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcWindow = m_pEmptyZoomOut->GetGlobalPos();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES  * 0.5f - rcWindow.sizeX * 0.5f;
		vPos.y = Y_RES * 2.0f / 3.0f - rcWindow.sizeY;

		SetLocalPos( vPos );
		SetGlobalPos( vPos );
	}

	return S_OK;
}

void CTutorialInterfaceZoomOut::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CTutorialInterfaceZoomOut::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	m_pFullZoomOut->SetVisibleSingle( TRUE );
}