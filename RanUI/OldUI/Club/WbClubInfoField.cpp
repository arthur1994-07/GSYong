#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../Util/CheckBox.h"
#include "../../InnerInterface.h"
#include "./WbClubInfoField.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CWbClubInfoField::CWbClubInfoField(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pText(NULL)
    , m_pOrder(NULL)
    , m_pSelect(NULL)
    , m_pListText(NULL)
    {
}

CWbClubInfoField::~CWbClubInfoField()
{
}

void CWbClubInfoField::CreateSubControl( const char* szBackKeyWord, const char* szTextKeyword, const char* szOrderKeyword, const char* szListKeyword, int nAlign )
{
    {
        CUIControl* pMouseOverLine = new CUIControl(m_pEngineDevice);
        pMouseOverLine->CreateSub( this, "SELCET_COUNTRY_WINDOW_NODE_SELECT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pMouseOverLine->SetVisibleSingle( FALSE );
        pMouseOverLine->SetUseRender( TRUE );	
        RegisterControl( pMouseOverLine );
        m_pSelect = pMouseOverLine;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, szBackKeyWord, UI_FLAG_XSIZE | UI_FLAG_YSIZE, BACK );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_LIST_FIELD_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, szTextKeyword, UI_FLAG_DEFAULT, TEXT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pText = pTextBox;
    }

    {
        CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
        pCheckBox->CreateSub( this, szOrderKeyword, UI_FLAG_DEFAULT, ORDER );
        pCheckBox->CreateSubControl( "SELCET_CLUB_LIST_FIELD_ASC", "SELCET_CLUB_LIST_FIELD_DESC" );
        RegisterControl( pCheckBox );
        m_pOrder = pCheckBox;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, szListKeyword, UI_FLAG_DEFAULT, LIST_TEXT );
        pTextBox->SetPartInterval( 5.0f );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( nAlign );
        RegisterControl( pTextBox );
        m_pListText = pTextBox;
    }

}

void CWbClubInfoField::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubInfoField::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case ORDER:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
            {
                club::ALIGN_ORDER_DES_ASC Order = GetFieldOrder();

                if ( Order == club::SQL_ASC )
                    SetFieldOrder( club::SQL_DESC );
                else
                    SetFieldOrder( club::SQL_ASC );
            }
        }
        break;
    };

}

void CWbClubInfoField::SetFiledText( const char* strFiledText )
{
    m_pText->ClearText();
    m_pText->AddTextNoSplit( strFiledText, NS_UITEXTCOLOR::WHITE );
}

void CWbClubInfoField::ClearList()
{
    m_pListText->ClearText();
}

void CWbClubInfoField::AddList( std::string& strText, DWORD dwID, const D3DCOLOR TextColor /* = NS_UITEXTCOLOR::WHITE */ )
{
    int nIndex = m_pListText->AddOneLineText( strText.c_str(), true, TextColor );
    m_pListText->SetTextData( nIndex, dwID );
}

void CWbClubInfoField::ResetOrder()
{
    m_pOrder->SetVisibleSingle( FALSE );
}

void CWbClubInfoField::SetCurListLine( int nPos )
{
    m_pListText->SetCurLine( nPos );
}

int CWbClubInfoField::GetVisibleListLine()
{
    return m_pListText->GetVisibleLine();
}

void CWbClubInfoField::SetSelectPos( int nIndex )
{
    UIRECT rcSelectPart;

    if ( m_pListText->GetPartInfo( nIndex, rcSelectPart ) )
    {
        rcSelectPart.left = GetGlobalPos().left;
        rcSelectPart.sizeX = GetLocalPos().sizeX;

        m_pSelect->SetGlobalPos( rcSelectPart );
        m_pSelect->SetVisibleSingle( TRUE );
        m_pSelect->SetDiffuse( D3DCOLOR_ARGB(0xFF,0x69,0x69,0x69) );
    }
    else
        m_pSelect->SetVisibleSingle( FALSE );
}

int CWbClubInfoField::GetSelectPos()
{
    return m_pListText->GetSelectPos();
}

void CWbClubInfoField::SetFieldOrder( club::ALIGN_ORDER_DES_ASC Order )
{
    m_pOrder->SetVisibleSingle( TRUE );

    switch ( Order )
    {
    case club::SQL_ASC:
        m_pOrder->SetCheckOn();
        break;

    case club::SQL_DESC:
        m_pOrder->SetCheckOff();
        break;
    };
}

club::ALIGN_ORDER_DES_ASC CWbClubInfoField::GetFieldOrder()
{
    if ( m_pOrder->IsChecked() )
        return club::SQL_ASC;
    else
        return club::SQL_DESC;
}

const DWORD CWbClubInfoField::GetTextData( int nIndex )
{
    return m_pListText->GetTextData( nIndex );
}

bool CWbClubInfoField::GetSelectPart( int nIndex, UIRECT& rcOut )
{
    return m_pListText->GetPartInfo( nIndex, rcOut );
}
