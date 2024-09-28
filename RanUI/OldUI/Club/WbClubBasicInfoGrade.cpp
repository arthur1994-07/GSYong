#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "./WbClubBasicInfoGrade.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2WORD_GRADE_POPUP_BTN_0 = "OK";
    const char* ID2WORD_GRADE_POPUP_BTN_1 = "Cancel";
}

const float CWbClubBasicInfoGrade::fRADIO_OFFSET = 5.0f;

CWbClubBasicInfoGrade::CWbClubBasicInfoGrade( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pLineBox(NULL)
    , m_nGradeIndex(-1)
{

    ID2WORD_GRADE_POPUP_BTN_0 = ID2GAMEWORD( "WB_CLUB_BASIC_INFO_GARDE_POPUP_BTN", 0 );
    ID2WORD_GRADE_POPUP_BTN_1 = ID2GAMEWORD( "WB_CLUB_BASIC_INFO_GARDE_POPUP_BTN", 1 );
}

CWbClubBasicInfoGrade::~CWbClubBasicInfoGrade()
{
}

void CWbClubBasicInfoGrade::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_BOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "ITEM_INFOR_TOOLTIP_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pLineBox = pLinBoxSmart;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_TITLE", UI_FLAG_DEFAULT );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pTitle = pTextBox;
    }

    {
        DEFAULT_RADIO_BUTTON* pBoardRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pBoardRadio->CreateSub(this,"WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_RADIO_BUTTON",UI_FLAG_DEFAULT, RADIO_START);
        pBoardRadio->CreateSubControl( "Temp", TEXT_ALIGN_LEFT| TEXT_ALIGN_CENTER_Y , "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_RADIO_TEXT", NS_UITEXTUTIL::WHITE, "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_RADIO_CHECK", "CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO", "CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
        RegisterTapButton(pBoardRadio);
        RegisterControl(pBoardRadio);
        m_pRadioButton[0] = pBoardRadio;

        for ( int i = 1; i < club::AUTHORITY_GRADE; i++ )
        {
            DEFAULT_RADIO_BUTTON* pBoardRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
            pBoardRadio->CreateSub(this,"WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_RADIO_BUTTON",UI_FLAG_DEFAULT, (RADIO_START+i));
            pBoardRadio->CreateSubControl( "Temp", TEXT_ALIGN_LEFT| TEXT_ALIGN_CENTER_Y , "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_RADIO_TEXT", NS_UITEXTUTIL::WHITE, "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_RADIO_CHECK", "CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO", "CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO");
            RegisterTapButton(pBoardRadio);
            RegisterControl(pBoardRadio);
            m_pRadioButton[i] = pBoardRadio;

            D3DXVECTOR2 vPos;
            vPos.x = m_pRadioButton[0]->GetLocalPos().left;
            vPos.y = m_pRadioButton[0]->GetLocalPos().top;

            vPos.y += (m_pRadioButton[0]->GetLocalPos().sizeY + fRADIO_OFFSET) * i;
            m_pRadioButton[i]->SetLocalPos( vPos );
        }
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
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_OK_BTN", UI_FLAG_DEFAULT, OK_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pOkButton = pRnButton;

        D3DXVECTOR2 vPos;
        vPos.x = 50.0f;
        vPos.y = m_pRadioButton[club::AUTHORITY_GRADE-1]->GetLocalPos().bottom + fRADIO_OFFSET;
        m_pOkButton->SetLocalPos( vPos );

        arg.text = ID2WORD_GRADE_POPUP_BTN_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_CANCEL_BTN", UI_FLAG_DEFAULT, CANCEL_BUTTON );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pCancelButton = pRnButton;

        vPos.x = 150.0f;
        vPos.y = m_pRadioButton[club::AUTHORITY_GRADE-1]->GetLocalPos().bottom + fRADIO_OFFSET;
        m_pCancelButton->SetLocalPos( vPos );
    }

    {
        float fSizeY = m_pCancelButton->GetLocalPos().bottom + fRADIO_OFFSET;

        const UIRECT& rcOriginLP = m_pLineBox->GetLocalPos();
        UIRECT rcNew = UIRECT( rcOriginLP.left, rcOriginLP.top, rcOriginLP.sizeX, fSizeY );
        m_pLineBox->SetLocalPos( rcNew );
    }
}

void CWbClubBasicInfoGrade::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubBasicInfoGrade::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    //CUIGroup::TranslateUIMessage( controlID, dwMsg );

    switch ( ControlID )
    {
    case OK_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                if ( IsSelected() )
                    m_pGaeaClient->SetClubMemberGrade( m_dwID, m_nGradeIndex );

                SetVisibleSingle( FALSE );
            }
        }
        break;

    case CANCEL_BUTTON:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                SetVisibleSingle( FALSE );
            }
        }
        break;
    };

    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
}

void CWbClubBasicInfoGrade::EventSelectedTap( UIGUID ControlID )
{
    int nIndex = ControlID - RADIO_START;

    m_nGradeIndex = nIndex;
}

void CWbClubBasicInfoGrade::SetGradeData( DWORD dwCharDbNum, const char* strCharName, DWORD dwGrade )
{
    m_dwID = dwCharDbNum;

    std::string strText = sc::string::format( ID2GAMEINTEXT("WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP_TITLE"), strCharName );
    m_pTitle->SetText( strText.c_str(), NS_UITEXTUTIL::WHITE );

    TapSelect( RADIO_START + dwGrade );

    for ( int i = 0; i < club::AUTHORITY_GRADE; i++ )
    {
        std::string& strGradeName = m_pGaeaClient->GetClubGradeName( i );
        m_pRadioButton[i]->SetText( strGradeName.c_str(), NS_UITEXTUTIL::WHITE );
    }
}

bool CWbClubBasicInfoGrade::IsSelected()
{
    if ( m_nGradeIndex < 0 || m_nGradeIndex >= club::AUTHORITY_GRADE )
        return false;

    return true;
}
