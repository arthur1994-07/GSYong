#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"

#include "./ProgressDisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CProgressDisplay::Begin_Attendance_Complete ()
{
    if ( !m_pCurInfo )
    {
        return;
    }

    SetPositionAlign( "PROGRESS_DISPLAY_ATTENDANCE_COMPLETE" );

    m_SchoolRecord.pSchoolBack->SetVisibleSingle( TRUE );

    const std::string strSCHOOLIMAGE = sc::string::format( "PROGRESS_DISPLAY_SCHOOL_IMAGE_%1%", m_pGaeaClient->GetCharacter()->m_wSchool );
    m_SchoolRecord.pSchoolImage->SetVisibleSingle( TRUE );
    m_SchoolRecord.pSchoolImage->SetImageResizing( strSCHOOLIMAGE.c_str() );

    m_SchoolRecord.pTextBox_Shadow->SetVisibleSingle( TRUE );

    m_SchoolRecord.pTextBox_Shadow->ClearText();
    m_SchoolRecord.pTextBox_Shadow->ChangeFontSize( 16 );
    m_SchoolRecord.pTextBox_Shadow->SetTextNoSplit( m_pCurInfo->strText.c_str(), NS_UITEXTCOLOR::WHITE );
}

void CProgressDisplay::Update_Attendance_Complete ( float fElapsedTime )
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
