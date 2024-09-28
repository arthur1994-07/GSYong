#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "./WbClubBasicInfoCommand.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float CWbClubBasicInfoCommand::fPOPUP_OFFSET = 3.0f;

CWbClubBasicInfoCommand::CWbClubBasicInfoCommand( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pLineBox(NULL)
    , m_pBanBtn(NULL)
    , m_pDelegateBtn(NULL)
    , m_pCDAuthorizeBtn(NULL)
    , m_pChangeGradeBtn(NULL)
    , m_bInitPostion(false)
{
}

CWbClubBasicInfoCommand::~CWbClubBasicInfoCommand()
{
}

void CWbClubBasicInfoCommand::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_COMMAND_POPUP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pLineBox = pLinBoxSmart;
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

        arg.text = ID2GAMEWORD_BASIC_INFO_BTN_0;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_COMMAND_POPUP_BAN_BTN", UI_FLAG_DEFAULT, BAN_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pBanBtn = pRnButton;

        arg.text = ID2GAMEWORD_BASIC_INFO_BTN_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_COMMAND_POPUP_DELEGATE_BTN", UI_FLAG_DEFAULT, DELEGATE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pDelegateBtn = pRnButton;

        arg.text = ID2GAMEWORD_BASIC_INFO_BTN_2;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_COMMAND_POPUP_CDAUTHORIZE_BTN", UI_FLAG_DEFAULT, CD_AUTHORIZE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pCDAuthorizeBtn = pRnButton;

        arg.text = ID2GAMEWORD_BASIC_INFO_BTN_3;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_COMMAND_POPUP_CHANGEGRADE_BTN", UI_FLAG_DEFAULT, CHANGE_GRADE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pChangeGradeBtn = pRnButton;
    }
}

void CWbClubBasicInfoCommand::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_bInitPostion )
    {
        m_bInitPostion = false;

        SetGlobalPos( D3DXVECTOR2( x+fPOPUP_OFFSET, y+fPOPUP_OFFSET ) );

        ShowButtons();
        CDAuthorizeBtnTextUpdate();

    }
}

void CWbClubBasicInfoCommand::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    //CUIGroup::TranslateUIMessage( controlID, dwMsg );

    switch ( ControlID )
    {

    case BAN_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_KICK_MEMBER"), m_strCharName.c_str() );
                m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_KICK );

                SetVisibleSingle( FALSE );
            }
        }
        break;

    case DELEGATE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_AUTHORITY_MEMBER"), m_strCharName.c_str() );
                m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_AUTHORITY );

                SetVisibleSingle( FALSE );
            }
        }
        break;

    case CD_AUTHORIZE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                DWORD dwCdCertifier = m_pGaeaClient->GetMyClub()->GetCdCertifier();
                if ( dwCdCertifier == 0 || m_pGaeaClient->GetMyClub()->IsMaster( dwCdCertifier ) )
                {
                    std::string strCombine = sc::string::format( ID2GAMEINTEXT("MODAL_CLUB_CD_AUTHORITY_MEMBER"), m_strCharName.c_str() );
                    m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_CD_AUTHORITY_TO_MEMBER );
                }
                else
                {
                    std::string strCombine = sc::string::format( ID2GAMEINTEXT("MODAL_CLUB_CD_AUTHORITY_MASTER"), m_strCharName.c_str() );
                    m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_CD_AUTHORITY_TO_MASTER );
                }

                SetVisibleSingle( FALSE );
            }
        }
        break;

    case CHANGE_GRADE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                m_pInterface->ShowClubwindowSetGradePopup();
                SetVisibleSingle( FALSE );
            }
        }
        break;
    };
}

void CWbClubBasicInfoCommand::SetVisibleSingle( BOOL isVisible )
{
    if ( isVisible == TRUE )
    {
        m_bInitPostion = true;
    }
    else
        AllUnShow();

    CUIGroup::SetVisibleSingle( isVisible );
}

bool CWbClubBasicInfoCommand::SetCommandData( DWORD dwCharDbNum, const char* strCharName, DWORD dwGrade )
{
    m_dwCharDbNum = dwCharDbNum;
    m_strCharName = sc::string::format( "%1%", strCharName );
    m_dwGrade = dwGrade;

    return CheckVisibleButtons();
}
bool CWbClubBasicInfoCommand::CheckVisibleButtons()
{
    bool bVisible = false;

    DWORD dwID = m_pGaeaClient->GetCharacter()->CharDbNum();

    if ( m_pGaeaClient->GetMyClub()->HaveMemberKickAuthority( dwID ) )
        bVisible = true;

    if ( m_pGaeaClient->GetMyClub()->IsMaster( dwID ) )
        bVisible = true;

    if ( m_pGaeaClient->GetMyClub()->HaveAuthChangeAuthority( dwID ) )
        bVisible = true;

    return bVisible;
}


void CWbClubBasicInfoCommand::ShowButtons()
{
    bool bVisible = false;

    DWORD dwID = m_pGaeaClient->GetCharacter()->CharDbNum();

    if ( m_pGaeaClient->GetMyClub()->HaveMemberKickAuthority( dwID ) )
    {
        m_pBanBtn->SetVisibleSingle( TRUE );

        bVisible = true;
    }
    else
        m_pBanBtn->SetVisibleSingle( FALSE );

    if ( m_pGaeaClient->GetMyClub()->IsMaster( dwID ) )
    {
        m_pDelegateBtn->SetVisibleSingle( TRUE );
        m_pCDAuthorizeBtn->SetVisibleSingle( TRUE );

        bVisible = true;
    }
    else
    {
        m_pDelegateBtn->SetVisibleSingle( FALSE );
        m_pCDAuthorizeBtn->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetMyClub()->HaveAuthChangeAuthority( dwID ) )
    {
        m_pChangeGradeBtn->SetVisibleSingle( TRUE );

        bVisible = true;
    }
    else
        m_pChangeGradeBtn->SetVisibleSingle( FALSE );

    if ( bVisible )
        m_pLineBox->SetVisibleSingle( TRUE );
    else
        m_pLineBox->SetVisibleSingle( FALSE );
}

void CWbClubBasicInfoCommand::CDAuthorizeBtnTextUpdate()
{
    DWORD dwCdCertifier = m_pGaeaClient->GetMyClub()->GetCdCertifier();
    if ( dwCdCertifier == 0 || m_pGaeaClient->GetMyClub()->IsMaster( dwCdCertifier ) )
        m_pCDAuthorizeBtn->SetText( ID2GAMEWORD_BASIC_INFO_BTN_2, NS_UITEXTCOLOR::WHITE );
    else
        m_pCDAuthorizeBtn->SetText( ID2GAMEWORD_BASIC_INFO_BTN_4, NS_UITEXTCOLOR::WHITE );
}

void CWbClubBasicInfoCommand::AllUnShow()
{
    m_pLineBox->SetVisibleSingle( FALSE );
    m_pBanBtn->SetVisibleSingle( FALSE );
    m_pDelegateBtn->SetVisibleSingle( FALSE );
    m_pCDAuthorizeBtn->SetVisibleSingle( FALSE );
    m_pChangeGradeBtn->SetVisibleSingle( FALSE );
}
