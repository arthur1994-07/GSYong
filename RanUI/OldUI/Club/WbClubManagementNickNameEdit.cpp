#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubManagementNickNameEdit.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_2 = "클럽 별명";
}

const int CWbClubManagementNickNameEdit::nMAX_NICKNAME_LENGTH = CHAR_SZNAME-1;

CWbClubManagementNickNameEdit::CWbClubManagementNickNameEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pTitle(NULL)
    , m_pLimitText(NULL)
    , m_pOkButton(NULL)
    , m_pCancelButton(NULL)
{
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_2 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 2);
}

CWbClubManagementNickNameEdit::~CWbClubManagementNickNameEdit()
{
}

void CWbClubManagementNickNameEdit::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT_BODY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT_TITLE" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pTitle = pTextBox;
        m_pTitle->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_2, NS_UITEXTCOLOR::WHITE );
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        m_pNickNameEditBox = new CUIMultiEditBoxSmart(m_pEngineDevice);
        m_pNickNameEditBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT_BODY", UI_FLAG_DEFAULT, EDIT_NICKNAME );
        m_pNickNameEditBox->CreateCarrat( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
        m_pNickNameEditBox->SetFont( pFont9 );
        m_pNickNameEditBox->SetLimitInput( nMAX_NICKNAME_LENGTH );
        m_pNickNameEditBox->SetAutoTurn( false );
        RegisterControl( m_pNickNameEditBox );
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT_LIMIT_TEXT" );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pLimitText = pTextBox;
    }

    {
        RnButton* pRnButton = NULL;
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont10Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = ID2WORD_GRADE_POPUP_BTN_0;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT_OK_BTN", UI_FLAG_DEFAULT, OK_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pOkButton = pRnButton;

        arg.text = ID2WORD_GRADE_POPUP_BTN_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT_CANCEL_BTN", UI_FLAG_DEFAULT, CANCEL_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pCancelButton = pRnButton;
    }
}

void CWbClubManagementNickNameEdit::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        int nLenth = m_pNickNameEditBox->GetEditLength();
        std::string strText = sc::string::format( ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 9 ),
            nLenth, nMAX_NICKNAME_LENGTH );

        m_pLimitText->SetText( strText.c_str() );
    }
}

void CWbClubManagementNickNameEdit::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case EDIT_NICKNAME:
        {
            TranslateMultiEditBoxSmartMsg( m_pNickNameEditBox, dwMsg );
        }
        break;

    case OK_BUTTON:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                std::string strNickName = sc::string::format( "%1%", m_pNickNameEditBox->GetEditString().GetString() );
                m_pGaeaClient->ReqClubNick( strNickName.c_str() );

                SetVisibleSingle( FALSE );
            }
        }
        break;

    case CANCEL_BUTTON:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                SetVisibleSingle( FALSE );
            }
        }
        break;
    };
}

void CWbClubManagementNickNameEdit::SetVisibleSingle( BOOL isVisible )
{
    CUIGroup::SetVisibleSingle( isVisible );

    if ( isVisible == TRUE )
    {
        m_pNickNameEditBox->ClearEdit();
    }
}

void CWbClubManagementNickNameEdit::TranslateMultiEditBoxSmartMsg( CUIMultiEditBoxSmart* pMultiEditBoxSmart, DWORD dwMsg )
{
    if( !pMultiEditBoxSmart ) return;

    if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
    {
        pMultiEditBoxSmart->EndEdit();
        pMultiEditBoxSmart->BeginEdit();
    }
    else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
    {
        pMultiEditBoxSmart->EndEdit();
    }

    pMultiEditBoxSmart->TranslateUIMessage( EDIT_NICKNAME, dwMsg );
}
