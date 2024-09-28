#include "StdAfx.h"
#include "BikeBoostWindow.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBikeBoostWindow::CBikeBoostWindow(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
{
}

CBikeBoostWindow::~CBikeBoostWindow ()
{
}

void CBikeBoostWindow::CreateSubControl ()
{
	m_pGageBack = CreateControl( "BOOSTER_GAGE_BACK", UI_FLAG_DEFAULT, BIKE_GAGE_BACK );
	m_pButtonBack = CreateControl( "BOOST_OFF_BUTTON", UI_FLAG_DEFAULT, BIKE_BUTTON_BACK );
	m_pGage = CreateControl( "BOOSTER_GAGE", UI_FLAG_DEFAULT, BIKE_GAGE );
	m_pButton = CreateControl( "BOOST_ON_BUTTON", UI_FLAG_DEFAULT, BIKE_BUTTON );

	m_pGage->SetFlowBar( m_pGage->GetGlobalPos(), m_pGage->GetTexturePos() );
	m_pGage->SetFlowBarType( CUIControl::FLOW_BAR_RIGHT );
}

CBasicButton* CBikeBoostWindow::CreateFlipButton ( char* szButton, char* szButtonFlip, char* szButtonOver, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver ( szButtonOver );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CBikeBoostWindow::CreateControl( CString strControl, WORD wAlignFlag, UIGUID WndID )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, strControl.GetString(), wAlignFlag, WndID );
	RegisterControl ( pControl );
	return pControl;
}


void CBikeBoostWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	if (pChar->VehicleUseBoost())
		m_pGage->FlowBarUpdate(1 - pChar->VehicleBoostTime()/6.0f);

	if (pChar->VehicleBoostUseDelay())
		m_pGage->FlowBarUpdate(1 - pChar->VehicleBoostDelay()/20.0f);


	if (!pChar->VehicleUseBoost() && !pChar->VehicleBoostUseDelay())
		m_pButton->SetVisibleSingle(TRUE);
	else
		m_pButton->SetVisibleSingle(FALSE);
}

void CBikeBoostWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( cID, dwMsg );

    NSGUI::ResetCharMoveBlock();
}