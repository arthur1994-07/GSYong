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

void CProgressDisplay::Update_Public_UI ( float fElapsedTime )
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

void CProgressDisplay::Begin_Public_UI_Ready()
{
	if ( !m_pCurInfo )
	{
		return;
	}

	SetPositionAlign( "PROGRESS_DISPLAY_CTF_READY" );

	m_PUBLIC_UI.pTextBox_Shadow->SetVisibleSingle( TRUE );
	m_PUBLIC_UI.pTextBox_Shadow->ClearText();
	m_PUBLIC_UI.pTextBox_Shadow->ChangeFontSize( 16 );
	m_PUBLIC_UI.pTextBox_Shadow->SetTextNoSplit( m_pCurInfo->strText.c_str(), NS_UITEXTCOLOR::WHITE );
}
