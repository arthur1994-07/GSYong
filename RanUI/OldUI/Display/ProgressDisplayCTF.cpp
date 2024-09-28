#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogic/Character/GLCharDefine.h"

#include "../../InnerInterface.h"

#include "./ProgressDisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CProgressDisplay::Begin_CTF_Start()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_CTF" );
    
    m_CTF.pBack->SetVisibleSingle( TRUE );
    m_CTF.pText->SetVisibleSingle( TRUE );

    m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_0" );
    m_CTF.pText->SetImageResizing( "PROGRESS_DISPLAY_CTF_TEXT_START" );
}

void CProgressDisplay::Begin_CTF_Stop()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

    m_CTF.pBack->SetVisibleSingle( TRUE );
    m_CTF.pText->SetVisibleSingle( TRUE );

    m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_0" );
    m_CTF.pText->SetImageResizing( "PROGRESS_DISPLAY_CTF_TEXT_STOP" );
}
void CProgressDisplay::Begin_CTF_Ready()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_CTF_READY" );

    m_CTF.pTextBox_Shadow->SetVisibleSingle( TRUE );
    m_CTF.pTextBox_Shadow->ClearText();
    m_CTF.pTextBox_Shadow->ChangeFontSize( 16 );
    m_CTF.pTextBox_Shadow->SetTextNoSplit( m_pCurInfo->strText.c_str(), NS_UITEXTCOLOR::WHITE );
}

void CProgressDisplay::Begin_CTF_CapturedBySM()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

    m_CTF.pBack->SetVisibleSingle( TRUE );
    m_CTF.pText->SetVisibleSingle( TRUE );
    m_CTF.pSchool->SetVisibleSingle( TRUE );

    const std::string strSCHOOLIMAGE = sc::string::format( "PROGRESS_DISPLAY_CTF_SCHOOL_%1%", SCHOOL_SM );
    const std::string strSCHOOLTEXT  = sc::string::format( "PROGRESS_DISPLAY_CTF_TEXT_CAPTURED_BY_%1%", SCHOOL_SM );

    m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_1" );
    m_CTF.pText->SetImageResizing( strSCHOOLTEXT.c_str() );
    m_CTF.pSchool->SetImageResizing( strSCHOOLIMAGE.c_str() );
}

void CProgressDisplay::Begin_CTF_CapturedByHA()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

    m_CTF.pBack->SetVisibleSingle( TRUE );
    m_CTF.pText->SetVisibleSingle( TRUE );
    m_CTF.pSchool->SetVisibleSingle( TRUE );

    const std::string strSCHOOLIMAGE = sc::string::format( "PROGRESS_DISPLAY_CTF_SCHOOL_%1%", SCHOOL_HA );
    const std::string strSCHOOLTEXT  = sc::string::format( "PROGRESS_DISPLAY_CTF_TEXT_CAPTURED_BY_%1%", SCHOOL_HA );

    m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_1" );
    m_CTF.pText->SetImageResizing( strSCHOOLTEXT.c_str() );
    m_CTF.pSchool->SetImageResizing( strSCHOOLIMAGE.c_str() );
}

void CProgressDisplay::Begin_CTF_CapturedByBH()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

    m_CTF.pBack->SetVisibleSingle( TRUE );
    m_CTF.pText->SetVisibleSingle( TRUE );
    m_CTF.pSchool->SetVisibleSingle( TRUE );

    const std::string strSCHOOLIMAGE = sc::string::format( "PROGRESS_DISPLAY_CTF_SCHOOL_%1%", SCHOOL_BH );
    const std::string strSCHOOLTEXT  = sc::string::format( "PROGRESS_DISPLAY_CTF_TEXT_CAPTURED_BY_%1%", SCHOOL_BH );

    m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_1" );
    m_CTF.pText->SetImageResizing( strSCHOOLTEXT.c_str() );
    m_CTF.pSchool->SetImageResizing( strSCHOOLIMAGE.c_str() );
}

void CProgressDisplay::Update_CTF ( float fElapsedTime )
{
	// TODO : RM #695
    //const float fTIME_DISAPPEAR_BEGIN = 5.0f;
	const float fTIME_DISAPPEAR_BEGIN = 1.5f;
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

void CProgressDisplay::ctf_hide_display()
{
	if(m_CTF.pBack   ) m_CTF.pBack->SetVisibleSingle( FALSE );
	if(m_CTF.pText   ) m_CTF.pText->SetVisibleSingle( FALSE );
	if(m_CTF.pSchool ) m_CTF.pSchool->SetVisibleSingle( FALSE );
}
