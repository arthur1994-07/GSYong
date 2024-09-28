#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "./WbClubStorageInputEdit.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CWbClubStorageInputEdit::nLIMIT_MONEY_CHAR = 11;

CWbClubStorageInputEdit::CWbClubStorageInputEdit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pTitle(NULL)
    , m_pStatic0(NULL)
    , m_pStatic1(NULL)
    , m_pText(NULL)
    , m_pEditBox(NULL)
    , m_pOkBtn(NULL)
    , m_pCancelBtn(NULL)
    , m_emType(NONE_TYPE)
{
}

CWbClubStorageInputEdit::~CWbClubStorageInputEdit ()
{
}

void CWbClubStorageInputEdit::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont12Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 12, _DEFAULT_FONT_SHADOW_FLAG );
        CD3DFontPar* pFont11Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 11, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_TITLE" );
        pTextBox->SetFont( pFont12Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pTitle = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_STATIC_0" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pStatic0 = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_STATIC_1" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pStatic1 = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_TEXT" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pText = pTextBox;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_EDITBOX_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont11 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 11, _DEFAULT_FONT_FLAG );

        CUIEditBox* pEditBox = new CUIEditBox( m_pEngineDevice );
        pEditBox->CreateSub( this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_EDITBOX", UI_FLAG_DEFAULT, EDIT_BOX );
        pEditBox->CreateCarrat( "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_EDITBOX_CARRAT", TRUE, UINT_MAX );
        pEditBox->SET_MONEY_FORMAT( 3, "," );
        pEditBox->SetLimitInput( nLIMIT_MONEY_CHAR );
        pEditBox->SetAlign( EDIT_RIGHT_ALIGN );
        pEditBox->SetFont( pFont11 );
        RegisterControl( pEditBox );
        m_pEditBox = pEditBox;
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

        arg.text = ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 0);
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_OK_BTN", UI_FLAG_DEFAULT, OK_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pOkBtn = pRnButton;

        arg.text = ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 1);
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_STORAGE_TAB_INPUT_EDIT_CANCEL_BTN", UI_FLAG_DEFAULT, CANCEL_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pCancelBtn = pRnButton;
    }
}

void CWbClubStorageInputEdit::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubStorageInputEdit::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case EDIT_BOX:
        {
            TranslateMultiEditBoxSmartMsg( m_pEditBox, dwMsg );
        }
        break;

    case OK_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                std::string strInputText = sc::string::format( "%1%", m_pEditBox->GetEditString().GetString() );
                LONGLONG lnInputMoney = _ttoi64( strInputText.c_str() );

                if ( m_emType == DEPOSIT_TYPE )
                {
                    LONGLONG lnHaveMoney = m_pGaeaClient->CharacterInvenMoney();
                    if ( lnHaveMoney < lnInputMoney )
                        return;

                    m_pGaeaClient->ReqClubStorageSaveMoney( lnInputMoney );
                }
                else if ( m_emType == WITHDRAW_TYPE )
                {
                    LONGLONG lnHaveMoney = m_pGaeaClient->ClubStorageMoney();
                    if ( lnHaveMoney < lnInputMoney )
                        return;

                    m_pGaeaClient->ReqClubStorageDrawMoney( lnInputMoney );
                }

                SetVisibleSingle( FALSE );
            }
        }
        break;

    case CANCEL_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                SetVisibleSingle( FALSE );
            }
        }
        break;
    };
}

void CWbClubStorageInputEdit::TranslateMultiEditBoxSmartMsg( CUIEditBox* pEditBox, DWORD dwMsg )
{
    if( !pEditBox ) return;

    if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
    {
        pEditBox->EndEdit();
        pEditBox->BeginEdit();
    }
    else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
    {
        pEditBox->EndEdit();
    }

    pEditBox->TranslateUIMessage( EDIT_BOX, dwMsg );
}

void CWbClubStorageInputEdit::SetVisibleSingle( BOOL bVisible )
{
    if ( bVisible == TRUE )
    {
        m_pEditBox->EndEdit();
        m_pEditBox->BeginEdit();
    }

    CUIGroup::SetVisibleSingle( bVisible );
}

void CWbClubStorageInputEdit::SetDepositType()
{
    m_emType = DEPOSIT_TYPE;

    m_pTitle->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 2), NS_UITEXTCOLOR::YELLOW );
    m_pStatic0->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 3), NS_UITEXTCOLOR::WHITE );
    m_pStatic1->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 4), NS_UITEXTCOLOR::WHITE );

    std::string strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_pGaeaClient->CharacterInvenMoney(), 3, "," );
    m_pText->SetText( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    m_pEditBox->ClearEdit();
}

void CWbClubStorageInputEdit::SetWithDrawType()
{
    m_emType = WITHDRAW_TYPE;

    m_pTitle->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 5), NS_UITEXTCOLOR::YELLOW );
    m_pStatic0->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 3), NS_UITEXTCOLOR::WHITE );
    m_pStatic1->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_STORAGE_INPUT_EDIT", 6), NS_UITEXTCOLOR::WHITE );

    std::string strText = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( m_pGaeaClient->ClubStorageMoney(), 3, "," );
    m_pText->SetText( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    m_pEditBox->ClearEdit();
}
