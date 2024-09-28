#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../Util/CheckBox.h"
#include "../../InnerInterface.h"
#include "./SelectClubListField.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectClubListField::CSelectClubListField(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pText(NULL)
    , m_pOrder(NULL)
    , m_pSelect(NULL)
    , m_pListText(NULL)
{
}

CSelectClubListField::~CSelectClubListField()
{
}

void CSelectClubListField::CreateSubControl( const char* szBackKeyWord, const char* szTextKeyword, const char* szOrderKeyword, const char* szListKeyword )
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, szBackKeyWord, UI_FLAG_XSIZE | UI_FLAG_YSIZE, BACK );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_LIST_FIELD_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, szTextKeyword, UI_FLAG_DEFAULT, TEXT );
        pTextBox->SetFont( pFont10Shadow );
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
        CUIControl* pMouseOverLine = new CUIControl(m_pEngineDevice);
        pMouseOverLine->CreateSub( this, "SELCET_COUNTRY_WINDOW_NODE_SELECT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pMouseOverLine->SetVisibleSingle( FALSE );
        pMouseOverLine->SetUseRender( TRUE );	
        RegisterControl( pMouseOverLine );
        m_pSelect = pMouseOverLine;
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, szListKeyword, UI_FLAG_DEFAULT, LIST_TEXT );
        pTextBox->SetPartInterval( 5.0f );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pListText = pTextBox;
    }

}

void CSelectClubListField::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectClubListField::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case BACK:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
            {
                wb::search::EM_ORDER_ASC_DSC Order;
                if ( GetFieldOrder() == wb::search::ASC )
                    Order = wb::search::DSC;
                else
                    Order = wb::search::ASC;

                m_pInterface->SetClubListOrder( GetFieldType(), Order );
                m_pInterface->SearchClubList();
            }
        }
        break;

    case ORDER:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
            {
                wb::search::EM_ORDER_ASC_DSC Order;
                if ( GetFieldOrder() == wb::search::ASC )
                    Order = wb::search::DSC;
                else
                    Order = wb::search::ASC;

                m_pInterface->SetClubListOrder( GetFieldType(), Order );

                m_pInterface->SearchClubList();
            }
        }
        break;

    case LIST_TEXT:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
            {
                m_pInterface->SelectClubList( m_pListText->GetSelectPos() );
            }
        }
        break;
    };

}

void CSelectClubListField::SetFieldType( wb::search::EM_ORDER_TYPE OrderType )
{
    m_Type = OrderType;
}

void CSelectClubListField::SetFiledText( const char* strFiledText )
{
    m_pText->SetText( strFiledText, NS_UITEXTCOLOR::WHITE );
}

void CSelectClubListField::ClearList()
{
    m_pListText->ClearText();
}

void CSelectClubListField::AddList( std::string& strText )
{
    m_pListText->AddOneLineText( strText.c_str(), FALSE, NS_UITEXTCOLOR::WHITE );
}

wb::search::EM_ORDER_TYPE CSelectClubListField::GetFieldType()
{
    return m_Type;
}

wb::search::EM_ORDER_ASC_DSC CSelectClubListField::GetFieldOrder()
{
    if ( m_pOrder->IsChecked() )
        return wb::search::ASC;
    else
        return wb::search::DSC;
}

void CSelectClubListField::SetFieldOrder( wb::search::EM_ORDER_ASC_DSC Order )
{
    m_pOrder->SetVisibleSingle( TRUE );

    switch ( Order )
    {
    case wb::search::ASC:
        m_pOrder->SetCheckOn();
        break;

    case wb::search::DSC:
        m_pOrder->SetCheckOff();
        break;
    };
}

void CSelectClubListField::ResetOrder()
{
    m_pOrder->SetVisibleSingle( FALSE );
}

void CSelectClubListField::SetCurListLine( int nPos )
{
    m_pListText->SetCurLine( nPos );
}

int CSelectClubListField::GetVisibleListLine()
{
    return m_pListText->GetVisibleLine();
}

void CSelectClubListField::SetSelectPos( int nIndex )
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
