#include "StdAfx.h"

#include "./PointShopTooltip.h"

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

CPointShopTooltip::CPointShopTooltip( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pBack(NULL)
    , m_pUpTextBoxTitle(NULL)
    , m_pUpTextBoxLine(NULL)
    , m_pUpTextBoxText(NULL)
    , m_pDownTextBoxTitle(NULL)
    , m_pDownTextBoxLine(NULL)
    , m_pDownTextBoxText(NULL)
{
}

CPointShopTooltip::~CPointShopTooltip()
{
}

void CPointShopTooltip::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_TOOLTIP_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_TOOLTIP_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pBack = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_UP_TEXT_BOX_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_UP_TEXT_BOX_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pUpTextBoxLine = pLinBoxSmart;

        pLinBoxSmart = new CBasicLineBoxSmart(m_pEngineDevice);
        pLinBoxSmart->CreateSub( this, "POINT_SHOP_DOWN_TEXT_BOX_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_DOWN_TEXT_BOX_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pDownTextBoxLine = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_UP_TEXT_BOX_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pUpTextBoxTitle = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_UP_TEXT_BOX_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pUpTextBoxText = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_DOWN_TEXT_BOX_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pDownTextBoxTitle = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub ( this, "POINT_SHOP_DOWN_TEXT_BOX_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pDownTextBoxText = pTextBox;
    }
}

void CPointShopTooltip::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CPointShopTooltip::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CPointShopTooltip::SetTextUpBoxTitle( std::string& strText, const D3DCOLOR& textColor )
{
    m_pUpTextBoxTitle->AddText( strText.c_str(), textColor );
}

void CPointShopTooltip::SetTextUpBoxText( std::string& strText, const D3DCOLOR& textColor )
{
    m_pUpTextBoxText->AddText( strText.c_str(), textColor );
}

void CPointShopTooltip::SetTextDownBoxTitle( std::string& strText, const D3DCOLOR& textColor )
{
    m_pDownTextBoxTitle->AddText( strText.c_str(), textColor );
}

void CPointShopTooltip::SetTextDownBoxText( std::string& strText, const D3DCOLOR& textColor )
{
    std::string strFind1 = "[";
    std::string strFind2 = "]";

    size_t idx1, idx2;

    idx1 = strText.find( strFind1 );
    if ( idx1 != -1 )
    {
        int nLength = 0;
        for ( ; strText.at(nLength) != '['; nLength++ );
        std::string strTemp = strText.substr( 0, nLength );
        int nIndex = m_pDownTextBoxText->AddText( strTemp.c_str(), textColor );

        idx2 = strText.find( strFind2 );

        if ( idx2 != -1 )
        {
            int nLength2 = 0;
            for ( ; strText.at( nLength + nLength2 ) != ']'; nLength2++ );
            strTemp = strText.substr( idx1, nLength2 + 1 );
            m_pDownTextBoxText->AddString( nIndex, strTemp.c_str(), NS_UITEXTCOLOR::GOLD );

            strTemp = strText.substr( idx2 + 1 );
            m_pDownTextBoxText->AddString( nIndex, strTemp.c_str(), textColor );
        }
    }
    else
        m_pDownTextBoxText->AddText( strText.c_str(), textColor );
}