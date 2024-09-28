#include "StdAfx.h"

#include "../../../SigmaCore/DebugSet.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "../Util/RnButton.h"
#include "./SelectCharacterPage.h"
#include "./SelectCountryConfirmModal.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


CSelectCountryConfirmModal::CSelectCountryConfirmModal(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIOuterWindow( pInterface, pEngineDevice )
    , m_pGaeaClient(pGaeaClient)
    , m_pMainText(NULL)
    , m_pOKButton(NULL)
    , m_pCancelButton(NULL)

    , m_emState( EM_BEFORE_SELECT )
{
}

CSelectCountryConfirmModal::~CSelectCountryConfirmModal()
{
}

void CSelectCountryConfirmModal::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_MODAL_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_MODAL_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_MODAL_BACK2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_MODAL_BACK_LINE2" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "SELCET_COUNTRY_MODAL_BODY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_COUNTRY_MODAL_BODY_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "SELCET_COUNTRY_MODAL_TEXT", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pMainText = pTextBox;
        m_pMainText->AddText( ID2GAMEEXTEXT( "SELECT_COUNTRY_MODAL_TEXT_1" ), NS_UITEXTCOLOR::YELLOW );
        m_pMainText->AddText( ID2GAMEEXTEXT( "SELECT_COUNTRY_MODAL_TEXT_2" ), NS_UITEXTCOLOR::WHITE );
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton* pRnButton = NULL;

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";
        //////////////////////////////////////////////////////////////////////////
        arg.pFont = pFont10Shadow;
        arg.text = sc::string::format( "%1%", ID2GAMEWORD( "SELECT_COUNTRY_MODAL_OK_BUTTON" ) );
        arg.dwColor = NS_UITEXTCOLOR::WHITE;

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"SELCET_COUNTRY_MODAL_OK_BUTTON", UI_FLAG_DEFAULT, OK_BUTTON);
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pOKButton = pRnButton;

        arg.text = sc::string::format( "%1%", ID2GAMEWORD( "SELECT_COUNTRY_MODAL_CANCEL_BUTTON" ) );

        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this,"SELCET_COUNTRY_MODAL_CANCEL_BUTTON", UI_FLAG_DEFAULT, CANCEL_BUTTON);
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pCancelButton = pRnButton;
    }
}

void CSelectCountryConfirmModal::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIOuterWindow::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectCountryConfirmModal::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIOuterWindow::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case OK_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                switch ( m_emState )
                {
                case EM_BEFORE_SELECT:
                    {
                        m_pInterface->ShowSelectCountryWindow();
                    }
                    break;
                    
                case EM_AFTER_SELECT:
                    {
                        m_pInterface->UiHideGroup( SELECT_COUNTRY_CONFIRM_MODAL );
                        SetBeforeSelectState();

                        m_pGaeaClient->ChangeUserCountry( m_CountyInfo.m_Code );
                    }
                    break;

                case EM_ESSENTIAL_SELECT_CLUB:
                    {
                        m_pInterface->ShowSelectClubWindow();
                    }
                    break;

                case EM_NONESSENTIAL_SELECT_CLUB:
                    {
                        m_pGaeaClient->GetGlobalStage()->SetGameJoin( FALSE );
                        m_pInterface->ShowSelectClubWindow();
                    }
                };
            }
        }
        break;

    case CANCEL_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pInterface->UiHideGroup( GetWndID() );

                switch ( m_emState )
                {
                case EM_BEFORE_SELECT:
                    {
                        if ( !m_pInterface->UiIsVisibleGroup( MODAL_WINDOW_OUTER ) )
                        {
                            m_pInterface->DoModal( ID2GAMEEXTEXT("NOT_SELECT_COUNTRY"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CONNECTCLOSED );
                        }
                    }
                    break;

                case EM_AFTER_SELECT:
                    {
                        m_pInterface->ShowSelectCountryWindow();
                    }
                    break;

                case EM_ESSENTIAL_SELECT_CLUB:
                    {
                        m_pGaeaClient->GetGlobalStage()->SetGameJoin( FALSE );
                    }
                    break;

                case EM_NONESSENTIAL_SELECT_CLUB:
                    {
                        m_pInterface->SendGameJoinMessage();
                    }
                    break;
                };
            }
        }
        break;
    }
}

void CSelectCountryConfirmModal::SetFocusControl()
{
    ResetFocusControl();
    if ( m_emState == EM_ESSENTIAL_SELECT_CLUB ||
        m_emState == EM_NONESSENTIAL_SELECT_CLUB )
        m_pInterface->UiShowGroupFocus( SELECT_CHARACTER_PAGE );
}

void CSelectCountryConfirmModal::SetBeforeSelectState()
{
    m_emState = EM_BEFORE_SELECT;

    SetMainText( ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_1"), ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_2") );
    SetOKButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_OK_BUTTON", 0) );
    SetCancelButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_CANCEL_BUTTON") );
}

void CSelectCountryConfirmModal::SetAfterSelectState(const wb::COUNTRY_INFO& CountryInfo )
{
    m_emState = EM_AFTER_SELECT;
    m_CountyInfo = CountryInfo;

    std::string strText = sc::string::format( ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_3"),
        CountryInfo.m_FullName );
    SetMainText( strText.c_str(), ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_4") );
    SetOKButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_OK_BUTTON", 1) );
    SetCancelButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_CANCEL_BUTTON") );
}

void CSelectCountryConfirmModal::SetMainText( const char* strMainText1, const char* strMainText2 )
{
    m_pMainText->ClearText();
    m_pMainText->AddText( strMainText1, NS_UITEXTCOLOR::YELLOW );
    m_pMainText->AddText( strMainText2, NS_UITEXTCOLOR::WHITE );
}

void CSelectCountryConfirmModal::SetOKButtonText( const char* strOKButtonText )
{
    m_pOKButton->SetText( strOKButtonText, NS_UITEXTCOLOR::WHITE );
}

void CSelectCountryConfirmModal::SetCancelButtonText( const char* strCancelButtonText )
{
    m_pCancelButton->SetText( strCancelButtonText, NS_UITEXTCOLOR::WHITE );
}

void CSelectCountryConfirmModal::SetEssentialSelectClub()
{
    m_emState = EM_ESSENTIAL_SELECT_CLUB;

    SetMainText( ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_5"), ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_6") );
    SetOKButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_OK_BUTTON", 2) );
    SetCancelButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_CANCEL_BUTTON") );
}
void CSelectCountryConfirmModal::SetNonEssentialSelectClub()
{
    m_emState = EM_NONESSENTIAL_SELECT_CLUB;

    SetMainText( ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_5"), ID2GAMEEXTEXT("SELECT_COUNTRY_MODAL_TEXT_6_1") );
    SetOKButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_OK_BUTTON", 2) );
    SetCancelButtonText( ID2GAMEWORD("SELECT_COUNTRY_MODAL_CANCEL_BUTTON", 1) );
}


