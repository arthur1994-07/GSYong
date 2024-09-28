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

void CProgressDisplay::Begin_RN_CDM_UI_Start()
{
	if ( !m_pCurInfo )
	{
		return;
	}

	SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

	m_CTF.pBack->SetVisibleSingle( TRUE );
	m_CTF.pText->SetVisibleSingle( TRUE );
	m_CTF.pSchool->SetVisibleSingle( TRUE );

	m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_1" );
	m_CTF.pText->SetImageResizing( "PROGRESS_DISPLAY_CDM_START" );
	m_CTF.pSchool->SetImageResizing( "PROGRESS_DISPLAY_CDM_START_END_ICON" );
}

void CProgressDisplay::Begin_RN_CDM_UI_End()
{
	if ( !m_pCurInfo )
	{
		return;
	}

	SetPositionAlign( "PROGRESS_DISPLAY_CTF" );

	m_CTF.pBack->SetVisibleSingle( TRUE );
	m_CTF.pText->SetVisibleSingle( TRUE );
	m_CTF.pSchool->SetVisibleSingle( TRUE );

	m_CTF.pBack->SetImageResizing( "PROGRESS_DISPLAY_CTF_BACK_1" );
	m_CTF.pText->SetImageResizing( "PROGRESS_DISPLAY_CDM_END" );
	m_CTF.pSchool->SetImageResizing( "PROGRESS_DISPLAY_CDM_START_END_ICON" );
}


void CProgressDisplay::update_CDM ( float fElapsedTime )
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

void CProgressDisplay::ResizeControl_CDM ()
{
	SetAlignFlag( UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );

	// Base의 사이즈를 구함
	std::string strBaseControl ( m_strPosition );

	CUIControl BaseControl(m_pEngineDevice); 
	BaseControl.Create ( 1, strBaseControl.c_str() );
	UIRECT rcBase = BaseControl.GetLocalPos();

	// 부모의 사이즈를 변경
	{
		SetLocalPos ( rcBase );

		// 800 x 600 을 기준으로한 정렬
		UIRECT rcNewBase = AlignMainControl ( rcBase );
		rcNewBase.right  = rcNewBase.left + rcNewBase.sizeX;
		rcNewBase.bottom = rcNewBase.top  + rcNewBase.sizeY;
		SetGlobalPos ( rcNewBase );
	}
}
