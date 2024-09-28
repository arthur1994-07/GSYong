#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../InnerInterface.h"

#include "./ProgressDisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CProgressDisplay::Begin_Activity_Complete ()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_ACTIVITY_COMPLETE" );

    m_SchoolRecord.pBackground->SetVisibleSingle( TRUE );
    m_SchoolRecord.pTextBox   ->SetVisibleSingle( TRUE );

    m_SchoolRecord.pTextBox->ClearText();
    m_SchoolRecord.pTextBox->ChangeFontSize( 12 );
    m_SchoolRecord.pTextBox->SetTextNoSplit( m_pCurInfo->strText.c_str(), NS_UITEXTCOLOR::YELLOW );
}

void CProgressDisplay::Update_Activity_Complete ( float fElapsedTime )
{
    const float fTIME_DISAPPEAR_BEGIN = 2.0f;
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

void CProgressDisplay::Begin_Activity_Progress ()
{
    if ( !m_pCurInfo )
    {
        return;
    }


    SetPositionAlign( "PROGRESS_DISPLAY_ACTIVITY_PROGRESS" );

    m_SchoolRecord.pBackground->SetVisibleSingle( TRUE );
    m_SchoolRecord.pTextBox   ->SetVisibleSingle( TRUE );

    m_SchoolRecord.pTextBox->ClearText();
    m_SchoolRecord.pTextBox->ChangeFontSize( 14 );
    m_SchoolRecord.pTextBox->SetTextNoSplit( m_pCurInfo->strText.c_str(), NS_UITEXTCOLOR::PALEGREEN );
}

void CProgressDisplay::Update_Activity_Progress ( float fElapsedTime )
{
    const float fTIME_DISAPPEAR_BEGIN = 2.0f;
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
