#include "StdAfx.h"

#include "../../InnerInterface.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../Util/RnButton.h"
#include "./RebirthWindowButton.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CRebirthWindowButton::CRebirthWindowButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup( pEngineDevice )
, m_pInterface(pInterface)
, m_pButton(NULL)
, m_pButtonText(NULL)
{
}

CRebirthWindowButton::~CRebirthWindowButton()
{
}

void CRebirthWindowButton::CreateSubControl()
{
    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton* pRnButton = NULL;

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"REBIRTH_WINDOW_REBIRTH_BUTTON",UI_FLAG_DEFAULT,BUTTON_BACK);
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pButton = pRnButton;

        CBasicTextBox* pTextBox = new CBasicTextBox( m_pEngineDevice );;
        pTextBox->CreateSub( this, "REBIRTH_WINDOW_REBIRTH_BUTTON_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pButtonText = pTextBox;
    }
}

void CRebirthWindowButton::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CRebirthWindowButton::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CRebirthWindowButton::SetText( std::string& strText )
{
    m_pButtonText->SetText( strText.c_str(), NS_UITEXTCOLOR::WHITE );
}

bool CRebirthWindowButton::IsFlip()
{
    if ( m_pButton->IsState( RnButton::MOUSEOVER ) )
        return true;

    return false;
}