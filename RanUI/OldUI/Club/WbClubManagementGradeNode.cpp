#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubManagementGradeNode.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_8 = "ÀúÀå";
}

const int CWbClubManagementGradeNode::nMAX_GRADENAME_LENGTH = CHR_ID_LENGTH-1;

CWbClubManagementGradeNode::CWbClubManagementGradeNode(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pIndexText(NULL)
    , m_pGradeEditBox(NULL)
    , m_pGradeName(NULL)
    , m_pSaveBtn(NULL)
{
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_8 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 8);
}

CWbClubManagementGradeNode::~CWbClubManagementGradeNode()
{
}

void CWbClubManagementGradeNode::CreateSubControl()
{
    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NODE_STATIC", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pIndexText = pTextBox;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NODE_EDIT_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        m_pGradeEditBox = new CUIMultiEditBoxSmart(m_pEngineDevice);
        m_pGradeEditBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NODE_EDIT", UI_FLAG_DEFAULT, EDIT_GRADE );
        m_pGradeEditBox->CreateCarrat( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
        m_pGradeEditBox->SetFont( pFont9 );
        m_pGradeEditBox->SetLimitInput( nMAX_GRADENAME_LENGTH );
        m_pGradeEditBox->SetAutoTurn( false );
        RegisterControl( m_pGradeEditBox );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NODE_EDIT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        pTextBox->SetVisibleSingle( FALSE );
        m_pGradeName = pTextBox;
    }

    {
        RnButton* pRnButton = NULL;
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont9Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_8;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NODE_SAVE_BTN", UI_FLAG_DEFAULT, SAVE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pSaveBtn = pRnButton;
    }
}

void CWbClubManagementGradeNode::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubManagementGradeNode::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case EDIT_GRADE:
        {
            TranslateMultiEditBoxSmartMsg( m_pGradeEditBox, dwMsg );
        }
        break;

    case SAVE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                std::string strEditString = sc::string::format( "%1%", m_pGradeEditBox->GetEditString().GetString() );

				if( strEditString != "" && strEditString.c_str() != NULL && strEditString.empty() == false )
					m_pGaeaClient->SetClubGradeName( m_nIndex, strEditString );
            }
        }
        break;
    };
}

void CWbClubManagementGradeNode::TranslateMultiEditBoxSmartMsg( CUIMultiEditBoxSmart* pMultiEditBoxSmart, DWORD dwMsg )
{
    if( !pMultiEditBoxSmart ) return;

    if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
    {
        pMultiEditBoxSmart->EndEdit();
        pMultiEditBoxSmart->BeginEdit();

        m_pGradeName->SetVisibleSingle( FALSE );
    }
    else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
    {
        pMultiEditBoxSmart->EndEdit();

        if ( pMultiEditBoxSmart->GetEditLength() == 0 )
            m_pGradeName->SetVisibleSingle( TRUE );
    }

    pMultiEditBoxSmart->TranslateUIMessage( EDIT_GRADE, dwMsg );
}

void CWbClubManagementGradeNode::SetGradeData( int nIndex )
{
    m_nIndex = nIndex;

    std::string strIndex = sc::string::format( "%1%", (nIndex+1) );
    m_pIndexText->SetText( strIndex.c_str(), NS_UITEXTCOLOR::WHITE );

    m_pGradeEditBox->ClearEdit();
    std::string& strGradeName = m_pGaeaClient->GetClubGradeName( m_nIndex );

    m_pGradeName->SetText( strGradeName.c_str(), NS_UITEXTCOLOR::WHITE );
    m_pGradeName->SetVisibleSingle( TRUE );
}