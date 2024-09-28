#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubAuthorityEditRow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CWbClubAuthorityEditRow::fCHECKBOX_OFFSET = 0.0f;

CWbClubAuthorityEditRow::CWbClubAuthorityEditRow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pAuthorityText(NULL)
{
    for ( int i = 0; i < club::AUTHORITY_GRADE; i++ )
        m_pCheckBox[i] = NULL;
}

CWbClubAuthorityEditRow::~CWbClubAuthorityEditRow()
{
}

void CWbClubAuthorityEditRow::CreateSubControl()
{

    {
        CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_ROW_TEXT" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pAuthorityText = pTextBox;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_ROW_CHECK_BACK" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );

        CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
        pCheckBox->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_ROW_CHECK", UI_FLAG_DEFAULT, (CHECK_START) );
        pCheckBox->CreateSubControl( "PRODUCT_WINDOW_LIST_CHECK", "PRODUCT_WINDOW_LIST_UNCHECK" );
        RegisterControl( pCheckBox );
        m_pCheckBox[0] = pCheckBox;

        for ( int i = 1; i < club::AUTHORITY_GRADE; i++ )
        {
            CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
            pCheckBox->CreateSub( this, "WB_CLUB_WINDOW_AUTHORITY_EDIT_ROW_CHECK", UI_FLAG_DEFAULT, (CHECK_START+i) );
            pCheckBox->CreateSubControl( "PRODUCT_WINDOW_LIST_CHECK", "PRODUCT_WINDOW_LIST_UNCHECK" );
            RegisterControl( pCheckBox );
            m_pCheckBox[i] = pCheckBox;

            D3DXVECTOR2 vPos;
            vPos.x = pControl->GetLocalPos().left;
            vPos.y = m_pCheckBox[0]->GetLocalPos().top;

            vPos.x += (pControl->GetLocalPos().sizeX + fCHECKBOX_OFFSET) * i;
            vPos.x += (pControl->GetLocalPos().sizeX - m_pCheckBox[0]->GetLocalPos().sizeX) * 0.5f;

            m_pCheckBox[i]->SetLocalPos( vPos );
        }
    }
}

void CWbClubAuthorityEditRow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubAuthorityEditRow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void CWbClubAuthorityEditRow::InitAuthorityData( const std::string& strAuthorityNAme )
{
    m_pAuthorityText->SetText( strAuthorityNAme.c_str(), NS_UITEXTCOLOR::WHITE );

    for ( int i = 0; i < club::AUTHORITY_GRADE; i++ )
        m_pCheckBox[ i ]->SetCheckOff();
}

void CWbClubAuthorityEditRow::CheckAuthority( size_t nGradeIndex )
{
    if ( nGradeIndex >= club::AUTHORITY_GRADE )
        return;

    m_pCheckBox[ nGradeIndex ]->SetCheckOn();
}

bool CWbClubAuthorityEditRow::IsAuthorityChecked( size_t nGradeIndex )
{
    if ( nGradeIndex >= club::AUTHORITY_GRADE )
        return false;

    return m_pCheckBox[ nGradeIndex ]->IsChecked();
}
