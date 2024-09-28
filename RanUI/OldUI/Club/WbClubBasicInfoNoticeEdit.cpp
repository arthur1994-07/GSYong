#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/UIMultiEditBox.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubBasicInfoNoticeEdit.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_1 = "클럽 공지 작성";
    const char* ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_2 = "클럽 소개 작성";
}

CWbClubInfoNoticeEdit::CWbClubInfoNoticeEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pTitle(NULL)
    , m_pLimitText(NULL)
    , m_pOkButton(NULL)
    , m_pCancelButton(NULL)
    , m_emType(NONE_TYPE)
{
    ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_1 = ID2GAMEWORD("WB_CLUB_WINDOW_INFO_NOTICE_EDIT", 1);
    ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_2 = ID2GAMEWORD("WB_CLUB_WINDOW_INFO_NOTICE_EDIT", 2);
}

CWbClubInfoNoticeEdit::~CWbClubInfoNoticeEdit()
{
}

void CWbClubInfoNoticeEdit::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_BODY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_TITLE" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pTitle = pTextBox;
        m_pTitle->SetText( ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_1, NS_UITEXTCOLOR::WHITE );
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        m_pNoticeEditBox = new CUIMultiEditBoxSmart(m_pEngineDevice);
        m_pNoticeEditBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_BODY", UI_FLAG_DEFAULT, NOTICE_EDIT );
        m_pNoticeEditBox->CreateCarrat( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
        m_pNoticeEditBox->SetFont( pFont9 );
        m_pNoticeEditBox->SetAutoTurn( true );
        RegisterControl( m_pNoticeEditBox );
    }

    {
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_LIMIT" );
        pTextBox->SetFont( pFont9 );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
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
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_OK_BTN", UI_FLAG_DEFAULT, OK_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pOkButton = pRnButton;

        arg.text = ID2WORD_GRADE_POPUP_BTN_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_CANCEL_BTN", UI_FLAG_DEFAULT, CANCEL_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pCancelButton = pRnButton;
    }
}

void CWbClubInfoNoticeEdit::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        int nLenth = m_pNoticeEditBox->GetEditLength();
        int nLimitInput = m_pNoticeEditBox->GetLimitInput();

        std::string strText = sc::string::format( ID2GAMEWORD( "POSTBOX_PAGE_SEND_TEXTBOX", 9 ),
            nLenth, nLimitInput );

        m_pLimitText->SetText( strText.c_str() );
    }
}

void CWbClubInfoNoticeEdit::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case NOTICE_EDIT:
        {
            TranslateMultiEditBoxSmartMsg( m_pNoticeEditBox, dwMsg );
        }
        break;

    case OK_BUTTON:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                PressOKButton();

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

void CWbClubInfoNoticeEdit::SetVisibleSingle( BOOL isVisible )
{
    CUIGroup::SetVisibleSingle( isVisible );

    if ( isVisible == TRUE )
    {
		m_pNoticeEditBox->ClearEdit();
		CString strtext = m_pGaeaClient->GetClubNotice().c_str();
		m_pNoticeEditBox->SetEditString(strtext);
		m_pNoticeEditBox->SetbUpdateChar(TRUE);
    }
}

void CWbClubInfoNoticeEdit::TranslateMultiEditBoxSmartMsg( CUIMultiEditBoxSmart* pMultiEditBoxSmart, DWORD dwMsg )
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

    pMultiEditBoxSmart->TranslateUIMessage( NOTICE_EDIT, dwMsg );
}

void CWbClubInfoNoticeEdit::SetClubNoticeType()
{
    m_emType = CLUB_NOTICE_TYPE;
    m_pTitle->SetText( ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_1, NS_UITEXTCOLOR::WHITE );
}

void CWbClubInfoNoticeEdit::SetClubIntroduceType()
{
    m_emType = CLUB_INTRODUCE_TYPE;
    m_pTitle->SetText( ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_2, NS_UITEXTCOLOR::WHITE );
}

void CWbClubInfoNoticeEdit::SetLimitInput( int nLimitInput )
{
    m_pNoticeEditBox->SetLimitInput( nLimitInput );
}

void CWbClubInfoNoticeEdit::PressOKButton()
{
    switch ( m_emType )
    {
    case CLUB_NOTICE_TYPE:
        {
            std::string strNotice = sc::string::format( "%1%", m_pNoticeEditBox->GetEditString().GetString() );
            if ( strNotice.empty() )
                return;

            m_pGaeaClient->ReqClubNotice( strNotice );
        }
        break;

    case CLUB_INTRODUCE_TYPE:
        {
            std::string strIntroduce = sc::string::format( "%1%", m_pNoticeEditBox->GetEditString().GetString() );
            if ( strIntroduce.empty() )
                return;

            m_pGaeaClient->ClubNewbieNoticeReq( strIntroduce );
        }
        break;
    };
}
