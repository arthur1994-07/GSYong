#include "pch.h"
#include "BasicTextButton.h"
#include "BasicTextButtonImage.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../../Enginelib/GUInterface/UITextControl.h"
#include "UITextControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicTextButton::CBasicTextButton(EngineDeviceMan* pEngineDevice)
    : CBasicButton(pEngineDevice)
	, m_bEnable ( true )
{
}

CBasicTextButton::~CBasicTextButton ()
{
}

void CBasicTextButton::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	if ( false == m_bEnable )
		return;

	CBasicButton::TranslateUIMessage( ControlID, dwMsg );
}

void CBasicTextButton::SetRadioButton ()
{
	SetUseDynamic ( FALSE );

	m_TextColor = NS_UITEXTCOLOR::DARKGRAY;
	m_TextFlipColor = NS_UITEXTCOLOR::WHITE;
}

void CBasicTextButton::SetGeneralButton ()
{
	SetUseDynamic ( TRUE );

	m_TextColor = NS_UITEXTCOLOR::WHITE;
	m_TextFlipColor = NS_UITEXTCOLOR::WHITE;
}

void CBasicTextButton::SetFlipPosition ()
{
	if ( m_pButtonFlip )
	{
		if ( !IsFlip () )
		{
			const UIRECT& rcBackPos = m_pButton->GetGlobalPos ();
			const UIRECT& rcBackLocalPos = m_pButton->GetLocalPos();

			m_pButtonFlip->SetLocalPos( D3DXVECTOR2 ( rcBackLocalPos.left + fBUTTON_CLICK, rcBackLocalPos.top + fBUTTON_CLICK ) );
			m_pButtonFlip->SetGlobalPos ( D3DXVECTOR2 ( rcBackPos.left + fBUTTON_CLICK, rcBackPos.top + fBUTTON_CLICK ) );

			const UIRECT& rcBackTextPos = m_pButtonTextBack->GetGlobalPos ();
			const UIRECT& rcBackTextLocalPos = m_pButtonTextBack->GetLocalPos();

			m_pButtonText->SetLocalPos( D3DXVECTOR2 ( rcBackTextLocalPos.left + fBUTTON_CLICK, rcBackTextLocalPos.top + fBUTTON_CLICK ) );
			m_pButtonText->SetGlobalPos ( D3DXVECTOR2 ( rcBackTextPos.left + fBUTTON_CLICK, rcBackTextPos.top + fBUTTON_CLICK ) );
		}
	}
}

void CBasicTextButton::ResetFlipPosition ()
{
	if ( m_pButtonFlip )
	{
		if ( IsFlip () )
		{
			const UIRECT& rcBackPos = m_pButton->GetGlobalPos ();
			const UIRECT& rcBackLocalPos = m_pButton->GetLocalPos();

			m_pButtonFlip->SetLocalPos ( D3DXVECTOR2 ( rcBackLocalPos.left, rcBackLocalPos.top ) );
			m_pButtonFlip->SetGlobalPos ( D3DXVECTOR2 ( rcBackPos.left, rcBackPos.top ) );

			const UIRECT& rcBackTextPos = m_pButtonTextBack->GetGlobalPos ();
			const UIRECT& rcBackTextLocalPos = m_pButtonTextBack->GetLocalPos();

			m_pButtonText->SetLocalPos ( D3DXVECTOR2 ( rcBackTextLocalPos.left, rcBackTextLocalPos.top ) );
			m_pButtonText->SetGlobalPos ( D3DXVECTOR2 ( rcBackTextPos.left, rcBackTextPos.top ) );
		}
	}
}

void CBasicTextButton::SetFlip ( BOOL bFlip )
{
	if ( m_pButton )	m_pButton->SetVisibleSingle ( !bFlip );
	if ( m_pButtonFlip )m_pButtonFlip->SetVisibleSingle ( bFlip );

	if ( m_pButtonText )
	{
		m_pButtonText->SetUseOverColor ( bFlip );
		
		if ( bFlip )	 m_pButtonText->SetOverColor ( 0, m_TextFlipColor );
		else			 m_pButtonText->SetOverColor ( 0, m_TextColor );
	}

	if ( IsUseDynamic() )
	{
		if ( bFlip )
		{
			SetFlipPosition ();
		}
	}	

	m_bFlip = bFlip;
}

HRESULT CBasicTextButton::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	의도적으로 CBasicButton::Render를 무시한 것입니다.
	// MEMO : CBasicButton::Render 없음.
	return CUIGroup::Render ( pd3dDevice );
}

void CBasicTextButton::SetOneLineText (const CString& strTemp)
{
	if ( m_pButtonText )
        m_pButtonText->SetOneLineText ( strTemp );
}