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
#include "../../../EngineLib/GUInterface/TipBox.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CMouseTip::CMouseTip(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pMain(NULL)
    , m_pLeftDir(NULL)
    , m_pRightDir(NULL)
    , m_pUpDir(NULL)
    , m_pDownDir(NULL)
    , m_pMouse(NULL)
    , m_pMouseButton(NULL)
    , m_pTipText(NULL)
    , m_fFlowTime( 0.0f )
{
}

CMouseTip::~CMouseTip(void)
{
}

void CMouseTip::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	{
		m_pMain = new CTipBox(m_pEngineDevice);;
		m_pMain->CreateSub( this, "TUTORIAL_BOX_MAIN", UI_FLAG_XSIZE | UI_FLAG_YSIZE, TUTORIAL_TIP_BOX );
		//m_pMain->CreateSub( this, "MOUSETIP_MAIN" );
		m_pMain->CreateMouseTipBox( "MOUSETIP_BOX" );
		RegisterControl ( m_pMain );
	}

	{
		CBasicTextBox* pTipText = new CBasicTextBox(m_pEngineDevice);
		pTipText->CreateSub ( this, "MOUSETIP_TEXT" );
		pTipText->SetFont ( pFont );
		pTipText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		RegisterControl ( pTipText );
		m_pTipText = pTipText;

		if ( m_pTipText )
		{
			CString strCombine;
			strCombine = ID2GAMEINTEXT("TUTORIAL_MOUSE_TIP_0");
			m_pTipText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
	}

	{
		CBasicTextBox* pTitleText = new CBasicTextBox(m_pEngineDevice);
		pTitleText->CreateSub ( this, "MOUSETIP_TEXT_TIP" );
		pTitleText->SetFont ( pFont );
		pTitleText->SetTextAlign ( TEXT_ALIGN_CENTER_X );
		RegisterControl ( pTitleText );
		m_pTitleText = pTitleText;

		if ( m_pTitleText )
		{
			CString strCombine;
			strCombine.Format ( "%s", "<TIP>" );
			m_pTitleText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::GOLD );
		}
	}

	{
		CUIControl* pLeftDir = new CUIControl(m_pEngineDevice);
		pLeftDir->CreateSub ( this, "MOUSETIP_LEFT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pLeftDir );
		m_pLeftDir = pLeftDir;
	}

	{
		CUIControl* pRightDir = new CUIControl(m_pEngineDevice);
		pRightDir->CreateSub ( this, "MOUSETIP_RIGHT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pRightDir );
		m_pRightDir = pRightDir;
	}

	{
		CUIControl* pUpDir = new CUIControl(m_pEngineDevice);
		pUpDir->CreateSub ( this, "MOUSETIP_UP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pUpDir );
		m_pUpDir = pUpDir;
	}

	{
		CUIControl* pDownDir = new CUIControl(m_pEngineDevice);
		pDownDir->CreateSub ( this, "MOUSETIP_DOWN", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pDownDir );
		m_pDownDir = pDownDir;
	}

	{
		CUIControl* pMouse = new CUIControl(m_pEngineDevice);
		pMouse->CreateSub ( this, "MOUSETIP_MOUSE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pMouse );
		m_pMouse = pMouse;
	}

	{
		CUIControl* pMouseButton = new CUIControl(m_pEngineDevice);
		pMouseButton->CreateSub ( this, "MOUSETIP_CENTER_BUTTON", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pMouseButton );
		m_pMouseButton = pMouseButton;
		m_pMouseButton->SetVisibleSingle( FALSE );
	}


	{
		CUIControl* pMouseScroll = new CUIControl(m_pEngineDevice);
		pMouseScroll->CreateSub ( this, "MOUSETIP_SCROLL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		RegisterControl ( pMouseScroll );
		m_pMouseScroll = pMouseScroll;
		m_pMouseScroll->SetVisibleSingle( FALSE );
	}

	//CUIControl::SetTexturePos ( NS_SKILL::GetIconTexurePosEx ( sICONINDEX ) );
	//CUIControl::SetTextureName ( strTextureName.GetString () );
}

void CMouseTip::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	m_fFlowTime += fElapsedTime;

	if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
	{
		m_pLeftDir->SetVisibleSingle( TRUE );
		m_pRightDir->SetVisibleSingle( TRUE );
		m_pUpDir->SetVisibleSingle( FALSE );
		m_pDownDir->SetVisibleSingle( FALSE );

		m_pMouseButton->SetVisibleSingle( TRUE );
		m_pMouseScroll->SetVisibleSingle( FALSE );
	}

	if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubFiveStep() )
	{
		m_pLeftDir->SetVisibleSingle( FALSE );
		m_pRightDir->SetVisibleSingle( FALSE );
		m_pUpDir->SetVisibleSingle( TRUE );
		m_pDownDir->SetVisibleSingle( TRUE );

		m_pMouseButton->SetVisibleSingle( TRUE );
		m_pMouseScroll->SetVisibleSingle( FALSE );

		CString strCombine;
		strCombine = ID2GAMEINTEXT("TUTORIAL_MOUSE_TIP_1");
		m_pTipText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
	}

	if ( m_pGaeaClient->GetTutorial()->IsMouseContNMoveStep()
		&& m_pGaeaClient->GetTutorial()->IsSubEightStep() )
	{
		m_pLeftDir->SetVisibleSingle( FALSE );
		m_pRightDir->SetVisibleSingle( FALSE );
		m_pUpDir->SetVisibleSingle( FALSE );
		m_pDownDir->SetVisibleSingle( FALSE );

		m_pMouseButton->SetVisibleSingle( FALSE );
		m_pMouseScroll->SetVisibleSingle( TRUE );

		CString strCombine;
		strCombine = ID2GAMEINTEXT("TUTORIAL_MOUSE_TIP_2");
		m_pTipText->SetOneLineText ( strCombine, NS_UITEXTCOLOR::WHITE );
	}

	/*if ( m_fFlowTime >= 0.8f )
	{
		if ( m_pMouseButton->IsVisible() )
			m_pMouseButton->SetVisibleSingle( FALSE );
		else
			m_pMouseButton->SetVisibleSingle( TRUE );

		m_fFlowTime = 0.0f;
	}*/
}

HRESULT CMouseTip::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = m_pInterface->UiGetResolution ();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	{
		const UIRECT& rcMouseTipWindow = GetGlobalPos ();

		D3DXVECTOR2 vPos;
		vPos.x = X_RES - rcMouseTipWindow.sizeX - 12.0f;
		vPos.y = ( Y_RES - rcMouseTipWindow.sizeY ) / 2.0f;
		SetGlobalPos ( vPos );
		SetLocalPos ( vPos );
	}

	return S_OK;
}

void CMouseTip::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
	{
		m_pInterface->SetDefaultPosInterface( MOUSETIP_WINDOW );
	}

	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}

void CMouseTip::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
}

