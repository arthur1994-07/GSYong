#include "StdAfx.h"
#include "RnSkillNomalToolTip.h"

#include "../../InnerInterface.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../SigmaCore/String/StringFormat.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


RnSkillNomalToolTip::RnSkillNomalToolTip( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )
    , m_pToolTipBackground( NULL )
    , m_pToolTipTextBox( NULL )
    , m_fVisibleTime( 0 )
{
}

void RnSkillNomalToolTip::CreateSubControl( const char* const szLineBoxInfo, const char* const szTextBoxInfo )
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
	pLinBoxSmart->CreateSub( this, szLineBoxInfo, UI_FLAG_XSIZE | UI_FLAG_YSIZE, ID_TOOLTIP_BACKGROUND );
	pLinBoxSmart->CreateSubControl( "POINT_SHOP_NOTIFY_FULL_CART_BACK_LINE" );
	RegisterControl( pLinBoxSmart );
	m_pToolTipBackground = pLinBoxSmart;

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub( this, szTextBoxInfo, UI_FLAG_DEFAULT, ID_TOOLTIP_TEXT );
	pTextBox->SetFont( pFont9 );
	pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	RegisterControl( pTextBox );
	m_pToolTipTextBox = pTextBox;
}

void RnSkillNomalToolTip::SetVisibleSingle( BOOL bVisible )
{
    m_fVisibleTime = 0;
    CUIGroup::SetVisibleSingle( bVisible );
}   


void RnSkillNomalToolTip::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}


void RnSkillNomalToolTip::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( m_pToolTipBackground->IsVisible() )
    {
        m_fVisibleTime += fElapsedTime;

        if ( m_fVisibleTime <= TOOTIP_VISIBLE_TIME )
        {
            float fPercent = m_fVisibleTime / TOOTIP_VISIBLE_TIME;
            WORD wDiffuse = static_cast<WORD>( 255 - ( 254 * fPercent * fPercent * fPercent ) );

            m_pToolTipBackground->SetDiffuseAlpha( wDiffuse );
            m_pToolTipTextBox->SetDiffuseAlpha( wDiffuse );

            if ( TOOTIP_VISIBLE_TIME < m_fVisibleTime )
                SetVisibleSingle( FALSE );
        }
        else
            SetVisibleSingle( FALSE );
    }

    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


int RnSkillNomalToolTip::AddString( int nIndex, const CString& strText, const D3DCOLOR& TextColor )
{
    return m_pToolTipTextBox->AddString( nIndex, strText, TextColor );
}


void RnSkillNomalToolTip::SetText( const CString& strText, const D3DCOLOR& TextColor )
{
    m_pToolTipTextBox->SetText( strText, TextColor );
}


int RnSkillNomalToolTip::AddText( const CString& strText, const D3DCOLOR& TextColor )
{
    return m_pToolTipTextBox->AddText( strText, TextColor );
}


void RnSkillNomalToolTip::ClearText()
{
    m_pToolTipTextBox->ClearText();
}
