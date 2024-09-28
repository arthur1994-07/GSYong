#include "StdAfx.h"
#include "RnSkillHelpToolTip.h"

#include "../../InnerInterface.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
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

RnSkillHelpToolTip::RnSkillHelpToolTip( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pToolTipBackground( NULL )
    , m_pToolTipTextBox( NULL )
{
}

void RnSkillHelpToolTip::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "RNSKILL_HELP_TOOLTIP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_TOOLTIP_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pToolTipBackground = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "RNSKILL_HELP_TOOLTIP_BG", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pToolTipTextBox = pTextBox;

    }

	std::string strOriginal = sc::string::format(ID2GAMEINTEXT("RNSKILL_TOOLTIP_HELP_TEXT" )," ");
	m_pToolTipTextBox->AddText( strOriginal.c_str() );

}


void RnSkillHelpToolTip::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void RnSkillHelpToolTip::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}
