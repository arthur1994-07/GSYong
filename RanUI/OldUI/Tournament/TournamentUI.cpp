#include "StdAfx.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../enginelib/DeviceMan.h"

#include "../../InnerInterface.h"

#include "./TournamentUI.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

TournamentUI::TournamentUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient  ( pGaeaClient )
, m_pInterface   ( pInterface )
, m_strPosition  ( "PROGRESS_DISPLAY" )
{
}

TournamentUI::~TournamentUI ()
{
}

void TournamentUI::CreateSubControl()
{
	//std::string strBaseControl ( GetControlNameEx() );

	// CTF
	{
		m_CTF.pBack = new CSwapImage(m_pEngineDevice);
		m_CTF.pBack->CreateSub ( this, "PROGRESS_DISPLAY_CTF_BACK_1", UI_FLAG_DEFAULT );
		m_CTF.pBack->SetVisibleSingle( FALSE );
		RegisterControl( m_CTF.pBack );

		m_CTF.pSchool = new CSwapImage(m_pEngineDevice);
		m_CTF.pSchool->CreateSub ( this, "TOURNAMENT_INFO_TEXT_WIN", UI_FLAG_DEFAULT );
		m_CTF.pSchool->SetVisibleSingle( FALSE );
		RegisterControl( m_CTF.pSchool );
	}
	Hide();
}

void TournamentUI::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_fAniTime += fElapsedTime;

	if(m_CTF.pBack->IsVisible() == TRUE)
	{
		Update_CTF(fElapsedTime);
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void TournamentUI::SetVisibleSingle( BOOL bVisible )
{
	//Blank
}

void TournamentUI::Show( const UI::EMTOURNAMENTDISPLAY_TYPE emType, const std::string& strText )
{

}

void TournamentUI::Begin ()
{
	CUIControl::SetVisibleSingle( TRUE );
}


void TournamentUI::Hide ()
{
	CUIControl::SetVisibleSingle( FALSE );
}

void TournamentUI::Update ()
{

}

void TournamentUI::ResizeControl ( const UI::EMTOURNAMENTDISPLAY_TYPE emType )
{

}

void TournamentUI::CTF::SetVisibleSingle( BOOL bVisible )
{
	if ( pBack ) pBack->SetVisibleSingle( bVisible );
	if ( pSchool ) pSchool->SetVisibleSingle( bVisible );
}

void TournamentUI::Begin_Tournament_Win()
{
	CUIControl::SetVisibleSingle( TRUE );

	SetDiffuse( D3DCOLOR_ARGB(255,255,255,255) );
	m_fAniTime = 0.0f;

	//ResizeControl_CTF();
	//SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

	m_CTF.SetVisibleSingle(TRUE);

	m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_1" );
	m_CTF.pSchool->SetImageResizing( "TOURNAMENT_INFO_TEXT_WIN" );
}

void TournamentUI::Begin_Tournament_Lose()
{

	CUIControl::SetVisibleSingle( TRUE );

	TournamentUI::m_pInterface->UiShowGroupFocus( TOURNAMENT_UI );

	SetDiffuse( D3DCOLOR_ARGB(255,255,255,255) );
	m_fAniTime = 0.0f;

	//ResizeControl_CTF();
	//SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

	m_CTF.SetVisibleSingle(TRUE);

	m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_1" );
	m_CTF.pSchool->SetImageResizing( "TOURNAMENT_INFO_TEXT_LOSE" );
}


void TournamentUI::Update_CTF( float fElapsedTime )
{
	const float fTIME_DISAPPEAR_BEGIN = 5.0f;
	const float fTIME_ALPHA_RATIO     = 0.7f;

	if ( m_fAniTime < fTIME_DISAPPEAR_BEGIN )
	{
		return;
	}

	float fAlphaRatio = (m_fAniTime - fTIME_DISAPPEAR_BEGIN) * fTIME_ALPHA_RATIO;

	if ( 1.0f <= fAlphaRatio )
	{
		SetDiffuseAlpha( 255 );
		Hide();
	}
	else
	{
		SetDiffuseAlpha( (WORD)(255.0f * (1.0f - fAlphaRatio)) );
	}
}

//----------------------------------------------------------------------------------------------------//

MyTournamentUI::MyTournamentUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: TournamentUI( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyTournamentUI::OnCreate() 
{
	TournamentUI::Create( TOURNAMENT_UI, "BASIC_WINDOW" );
	TournamentUI::CreateSubControl();
	TournamentUI::m_pInterface->UiRegisterControl( this );
	TournamentUI::m_pInterface->UiShowGroupTop( TOURNAMENT_UI );
	TournamentUI::SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );
	//TournamentUI::SetVisibleSingle( FALSE );
}

void MyTournamentUI::OnVisible(bool visible)
{
	if(visible)
	{
		TournamentUI::m_pInterface->UiShowGroupFocus( TOURNAMENT_UI );
	}
	else
	{
		TournamentUI::m_pInterface->UiHideGroup(TOURNAMENT_UI);
	}
}


void MyTournamentUI::OnUpdate()
{
	TournamentUI::Update();
}

const UIRECT& MyTournamentUI::GetGlobalPos()
{
	return TournamentUI::GetGlobalPos();
}

void MyTournamentUI::WinLoseScoreVisible(bool visible)
{
	if(visible)
		TournamentUI::Begin_Tournament_Win();
	else
		TournamentUI::Begin_Tournament_Lose();
}


