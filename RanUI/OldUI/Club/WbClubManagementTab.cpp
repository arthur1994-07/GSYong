#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/SmartComboBoxRollOver.h"

#include "./WbClubManagementNickNameEdit.h"
#include "./WbClubManagementGradeNameEdit.h"
#include "./WbClubManagementAuthorityEdit.h"
#include "./WbClubBasicInfoNoticeEdit.h"
#include "./WbClubManagementTab.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_0 = "별명";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_1 = "수정";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_3 = "계급";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_4 = "이름 수정";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_5 = "권한 수정";
    //const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_9 = "공개";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_10 = "탈퇴";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_11 = "클럽 탈퇴";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_12 = "이하";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_13 = "일 이상";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_14 = "마크";
    //const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_15 = "소개";
    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_16 = "클럽 해산";
	const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_17 = "해산 취소";

	const char* ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_BREAK_UP_ESTIMATED_TIME = "해산까지 남은시간 : %1%년 %2%월 %3%일 %4%시 %5%분";

    //const char* ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_NEWCOMER = "신입생들에게 클럽 공개";
    //const char* ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_OTHER = "타 클럽원에게 공개";
    const char* ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN = "일정기간 미접속시 자동 추방";
    //const char* ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE = "신입생들에게 공개되는 클럽소개 글을 입력하세요.";

    const char* ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_SAVE_BTN = "저장";
}

const int CWbClubManagementTab::nMAX_AUTO_BAN_DAY = 30;


CWbClubManagementTab::CWbClubManagementTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pNickNameText(NULL)
    , m_pNickNameBtn(NULL)
    , m_pNickNameEdit(NULL)
    , m_pGradeNameEdit(NULL)
    , m_pAuthorityEdit(NULL)
    , m_pGradeEditBtn(NULL)
    , m_pAuthorityEditBtn(NULL)
    //, m_pCheckPublicToNewcomer(NULL)
    //, m_pCheckPublicToOther(NULL)
    //, m_pPublicSaveBtn(NULL)
    , m_pCheckAutoBan(NULL)
    , m_pAutoBanSaveBtn(NULL)
    , m_pAutoBanGradeText(NULL)
    , m_pAutoBanGradeRollOver(NULL)
    , m_pAutoBanDayText(NULL)
    , m_pAutoBanDayRollOver(NULL)
    , m_pAutoBanDayBtn(NULL)
    , m_pLeaveClubBtn(NULL)
    , m_pBreakUpClubBtn(NULL)
	, m_pBreakUpClubText(NULL)
	, m_pClubMarkBackground(NULL)
    , m_pClubMark(NULL)
    , m_pClubMarkEditBtn(NULL)
    //, m_pIntroduceText(NULL)
    //, m_pIntroduceEditBtn(NULL)
    , m_nSelectGradeIndex(-1)
    , m_nSelectDayIndex(-1)
{
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_0 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 0);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_1 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 1);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_3 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 3);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_4 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 4);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_5 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 5);
    //ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_9 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 9);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_10 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 10);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_11 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 11);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_12 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 12);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_13 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 13);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_14 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 14);
    //ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_15 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 15);
    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_16 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 16);
	ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_17 = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 17);

	ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_BREAK_UP_ESTIMATED_TIME = ID2GAMEINTEXT("CLUB_BREAKUP_ESTIMATED_TIME");

    //ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_NEWCOMER = ID2GAMEINTEXT("WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_NEWCOMER");
    //ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_OTHER = ID2GAMEINTEXT("WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_OTHER");
    ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN = ID2GAMEINTEXT("WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN");
    //ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE = ID2GAMEINTEXT("WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE");

    ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_SAVE_BTN = ID2GAMEWORD("WB_CLUB_WINDOW_MANAGEMENT_TAB", 8);
}

CWbClubManagementTab::~CWbClubManagementTab()
{
}

void CWbClubManagementTab::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_SEPLINE_0", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_SEPLINE_1", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_SEPLINE_2", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_SEPLINE_3", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE_2" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_SEPLINE_4", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE_2" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_SEPLINE_5", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE_2" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_TAB_SEPLINE_6", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE_2" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_STATIC" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_0,
            NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_BACK_STATIC" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_3,
            NS_UITEXTCOLOR::WHITE );

        //pTextBox = new CBasicTextBox(m_pEngineDevice);
        //pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_BACK_STATIC" );
        //pTextBox->SetFont( pFont10Shadow );
        //pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        //RegisterControl( pTextBox );
        //pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_9,
        //    NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_LEAVE_BACK_STATIC" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_10,
            NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_MARK_STATIC" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_14,
            NS_UITEXTCOLOR::WHITE );

        /*pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE_STATIC" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_15,
            NS_UITEXTCOLOR::WHITE );*/
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_BACK_STATIC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont11Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 11, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_TEXT" );
        pTextBox->SetFont( pFont11Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pNickNameText = pTextBox;
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

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_BTN", UI_FLAG_DEFAULT, NICK_NAME_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pNickNameBtn = pRnButton;
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

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_4;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NAME_BTN", UI_FLAG_DEFAULT, GRADE_NAME_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pGradeEditBtn = pRnButton;
        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_5;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_AUTHORITY_BTN", UI_FLAG_DEFAULT, AUTHORITY_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pAuthorityEditBtn = pRnButton;
    }

    {
        //CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
        //pCheckBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_NEWCOMER_CHECK", UI_FLAG_DEFAULT, CHECK_TO_NEWCOMER );
        //pCheckBox->CreateSubControl( "PRODUCT_WINDOW_LIST_CHECK", "PRODUCT_WINDOW_LIST_UNCHECK" );
        //RegisterControl( pCheckBox );
        //m_pCheckPublicToNewcomer = pCheckBox;

        //CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        //CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        //pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_NEWCOMER_TEXT" );
        //pTextBox->SetFont( pFont10Shadow );
        //pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        //RegisterControl( pTextBox );
        //pTextBox->SetText( ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_NEWCOMER,
        //    NS_UITEXTCOLOR::WHITE );

        //pCheckBox = new CCheckBox( m_pEngineDevice );
        //pCheckBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_OTHER_CHECK", UI_FLAG_DEFAULT, CHECK_TO_OTHER );
        //pCheckBox->CreateSubControl( "PRODUCT_WINDOW_LIST_CHECK", "PRODUCT_WINDOW_LIST_UNCHECK" );
        //RegisterControl( pCheckBox );
        //m_pCheckPublicToOther = pCheckBox;

        //pTextBox = new CBasicTextBox(m_pEngineDevice);
        //pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_OTHER_TEXT" );
        //pTextBox->SetFont( pFont10Shadow );
        //pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        //RegisterControl( pTextBox );
        //pTextBox->SetText( ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_TO_OTHER,
        //    NS_UITEXTCOLOR::WHITE );
    }

    {
        //RnButton* pRnButton = NULL;
        //CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        //RnButton::CreateArg arg;
        ////////////////////////////////////////////////////////////////////////////
        //arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        //arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        //arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        //arg.pFont = pFont10Shadow;
        //arg.dwColor = NS_UITEXTCOLOR::WHITE;
        ////////////////////////////////////////////////////////////////////////////

        //arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_SAVE_BTN;
        //pRnButton = new RnButton(m_pEngineDevice);
        //pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_PUBLIC_SAVE_BTN", UI_FLAG_DEFAULT, PUBLIC_SAVE_BTN );
        //pRnButton->CreateSubControl(arg);
        //RegisterControl(pRnButton);
        //m_pPublicSaveBtn = pRnButton;
    }

    {
        CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
        pCheckBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_CHECK", UI_FLAG_DEFAULT, CHECK_AUTO_BAN );
        pCheckBox->CreateSubControl( "PRODUCT_WINDOW_LIST_CHECK", "PRODUCT_WINDOW_LIST_UNCHECK" );
        RegisterControl( pCheckBox );
        m_pCheckAutoBan = pCheckBox;

        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_TEXT" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN,
            NS_UITEXTCOLOR::WHITE );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_GRADE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_GRADE_TEXT" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pAutoBanGradeText = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_GRADE_STATIC" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_12, NS_UITEXTCOLOR::WHITE );

        CBasicButton* pButton = new CBasicButton( m_pEngineDevice );
        pButton->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_GRADE_BTN", UI_FLAG_DEFAULT, AUTO_BAN_GRADE_BTN );
        pButton->CreateFlip( "CHARACTER_WINDOW_TITLE_BUTTON_OVER", CBasicButton::MOUSEIN_FLIP );
        RegisterControl( pButton );
        m_pAutoBanGradeBtn = pButton;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_DAY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_DAY_TEXT" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pAutoBanDayText = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_DAY_STATIC" );
        pTextBox->SetFont( pFont10Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_13, NS_UITEXTCOLOR::WHITE );

        CBasicButton* pButton = new CBasicButton( m_pEngineDevice );
        pButton->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_DAY_BTN", UI_FLAG_DEFAULT, AUTO_BAN_DAY_BTN );
        pButton->CreateFlip( "CHARACTER_WINDOW_TITLE_BUTTON_OVER", CBasicButton::MOUSEIN_FLIP );
        RegisterControl( pButton );
        m_pAutoBanDayBtn = pButton;
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

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_SAVE_BTN;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_SAVE_BTN", UI_FLAG_DEFAULT, AUTO_BAN_SAVE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pAutoBanSaveBtn = pRnButton;
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

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_11;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_LEAVE_CLUB_BTN", UI_FLAG_DEFAULT, LEAVE_CLUB_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pLeaveClubBtn = pRnButton;
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

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_16;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_BREAK_UP_CLUB_BTN", UI_FLAG_DEFAULT, BREAK_UP_CLUB_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pBreakUpClubBtn = pRnButton;

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_BREAK_UP_CLUB_TEXT" );
		pTextBox->SetFont( pFont10Shadow );
		pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTextBox );
		pTextBox->SetText( ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN,
			NS_UITEXTCOLOR::RED );
		m_pBreakUpClubText = pTextBox;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_CLUB_MARK" );
        RegisterControl( pControl );
        pControl->SetUseRender( TRUE );
        m_pClubMark = pControl;

		m_pClubMarkBackground = new CSwapImage( m_pEngineDevice );;
		m_pClubMarkBackground->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_CLUB_MARK_BACKGROUND" );
		RegisterControl( m_pClubMarkBackground );

		//m_pClubMarkBackground->SetImage( "WHITE_LINE_BACKGROUND" );
		m_pClubMarkBackground->SetDiffuse( GLCONST_CHAR::dwClubMarkARGB );
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

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_CLUB_MARK_EDIT_BTN", UI_FLAG_DEFAULT, CLUB_MARK_EIDT_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pClubMarkEditBtn = pRnButton;
    }

    {
        /*CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE_NOTICE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POINT_SHOP_ITEM_SEARCH_BACK_LINE" );
        RegisterControl( pLinBoxSmart );

        pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE_CONTENT_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "POSTBOX_PAGE_SEND_LINEBOX_TEXINFO" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE_NOTICE" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        pTextBox->SetText( ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE,
            NS_UITEXTCOLOR::YELLOW );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE_CONTENT" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pIntroduceText = pTextBox;*/
    }

    {
        /*RnButton* pRnButton = NULL;
        CD3DFontPar* pFont10Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_SHADOW_FLAG );

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont10Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE_EDIT_BTN", UI_FLAG_DEFAULT, INTRODUCE_EDIT_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        m_pIntroduceEditBtn = pRnButton;*/
    }

    {
        CSmartComboBoxRollOver* pComboBoxRollOver = new CSmartComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, AUTO_BAN_GRADE_ROLLOVER );
        pComboBoxRollOver->CreateSmartComboBoxRollOver( "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_GRADE_ROLLOVER", "CHARACTER_WINDOW_TITLE_COMBO_ROLLOVER_TEXINFO" );
        pComboBoxRollOver->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pAutoBanGradeRollOver = pComboBoxRollOver;

        pComboBoxRollOver = new CSmartComboBoxRollOver(m_pEngineDevice);
        pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, AUTO_BAN_DAY_ROLLOVER );
        pComboBoxRollOver->CreateSmartComboBoxRollOver( "WB_CLUB_WINDOW_MANAGEMENT_AUTO_BAN_DAY_ROLLOVER", "CHARACTER_WINDOW_TITLE_COMBO_ROLLOVER_TEXINFO" );
        pComboBoxRollOver->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
        pComboBoxRollOver->SetVisibleSingle( FALSE );
        RegisterControl( pComboBoxRollOver );
        m_pAutoBanDayRollOver = pComboBoxRollOver;
    }

    {
        CWbClubManagementNickNameEdit* pNickNameEdit = new CWbClubManagementNickNameEdit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pNickNameEdit->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_NICKNAME_EDIT" );
        pNickNameEdit->CreateSubControl();
        RegisterControl( pNickNameEdit );
        pNickNameEdit->SetVisibleSingle( FALSE );
        m_pNickNameEdit = pNickNameEdit;
    }

    {
        CWbClubManagementGradeNameEdit* pGradeNameEdit = new CWbClubManagementGradeNameEdit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pGradeNameEdit->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_GRADE_NAME_EDIT" );
        pGradeNameEdit->CreateSubControl();
        RegisterControl( pGradeNameEdit );
        pGradeNameEdit->SetVisibleSingle( FALSE );
        m_pGradeNameEdit = pGradeNameEdit;
    }

    {
        CWbClubManagementAuthorityEdit* pAuthorityEdit = new CWbClubManagementAuthorityEdit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pAuthorityEdit->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_AUTHORITY_EDIT" );
        pAuthorityEdit->CreateSubControl();
        RegisterControl( pAuthorityEdit );
        pAuthorityEdit->SetVisibleSingle( FALSE );
        m_pAuthorityEdit = pAuthorityEdit;
    }

    {
        CWbClubInfoNoticeEdit* pNoticeEdit = new CWbClubInfoNoticeEdit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        //pNoticeEdit->CreateSub( this, "WB_CLUB_WINDOW_MANAGEMENT_INTRODUCE_EDIT" );
        pNoticeEdit->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT" );
        pNoticeEdit->CreateSubControl();
        RegisterControl( pNoticeEdit );
        pNoticeEdit->SetClubIntroduceType();
        pNoticeEdit->SetLimitInput( EMCLUB_NOTICE_LEN - 1 );
        pNoticeEdit->SetVisibleSingle( FALSE );
        m_pClubIntroduceEdit = pNoticeEdit;
    }
}

void CWbClubManagementTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWbClubManagementTab::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    if ( m_pNickNameEdit->IsVisible() ||
        m_pGradeNameEdit->IsVisible() ||
        m_pAuthorityEdit->IsVisible() )
        return;

    switch ( ControlID )
    {
    case NICK_NAME_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pNickNameEdit->SetVisibleSingle( TRUE );
            }
        }
        break;

    case GRADE_NAME_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pGradeNameEdit->SetVisibleSingle( TRUE );
            }
        }
        break;

    case AUTHORITY_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pAuthorityEdit->SetVisibleSingle( TRUE );
            }
        }
        break;

    //case PUBLIC_SAVE_BTN:
    //    {
    //        if ( dwMsg & UIMSG_RNBUTTON_CLICK )
    //        {
    //            SavePublic();
    //        }
    //    }
    //    break;

    case AUTO_BAN_SAVE_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                if ( !m_pAutoBanGradeRollOver->IsVisible() &&
                    !m_pAutoBanDayRollOver->IsVisible() )
                    SaveAutoBan();
            }
        }
        break;

    case AUTO_BAN_GRADE_BTN:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_pAutoBanGradeRollOver->SetVisibleSingle( !m_pAutoBanGradeRollOver->IsVisible() );
            }
        }
        break;

    case AUTO_BAN_DAY_BTN:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                m_pAutoBanDayRollOver->SetVisibleSingle( !m_pAutoBanDayRollOver->IsVisible() );
            }
        }
        break;

    case AUTO_BAN_GRADE_ROLLOVER:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = m_pAutoBanGradeRollOver->GetSelectIndex();
                if ( nIndex < 0 )
                    break;

                m_nSelectGradeIndex = nIndex;

                m_pAutoBanGradeText->SetText( m_pAutoBanGradeRollOver->GetSelectText( m_nSelectGradeIndex ),
                    NS_UITEXTCOLOR::WHITE );

                m_pAutoBanGradeRollOver->SetVisibleSingle( FALSE );
            }
        }
        break;

    case AUTO_BAN_DAY_ROLLOVER:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                int nIndex = m_pAutoBanDayRollOver->GetSelectIndex();
                if ( nIndex < 0 )
                    break;

                m_nSelectDayIndex = nIndex;

                m_pAutoBanDayText->SetText( m_pAutoBanDayRollOver->GetSelectText( m_nSelectDayIndex ),
                    NS_UITEXTCOLOR::WHITE );

                m_pAutoBanDayRollOver->SetVisibleSingle( FALSE );
            }
        }
        break;

    case CHECK_AUTO_BAN:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                if ( m_pCheckAutoBan->IsChecked() )
                    EnableAutoban();
                else
                    DisableAutoban();
            }
        }
        break;

    case LEAVE_CLUB_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                LeaveClub();
            }
        }
        break;

    case BREAK_UP_CLUB_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
				if( m_pGaeaClient->GetMyClub()->IsRegDissolution() )
					m_pInterface->DoModal( ID2GAMEINTEXT("CLUB_BREAKUP_CANCEL"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_BREAKUP_CANCEL );
				else
					m_pInterface->DoModal( ID2GAMEINTEXT("CLUB_BREAKUP"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_BREAKUP );					
            }

			if( m_pGaeaClient->GetMyClub()->IsRegDissolution() )
			{
				ChangeDissolutionTime();
			}
        }
        break;

    case CLUB_MARK_EIDT_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                ChangeClubMark();
            }
        }
        break;

    case INTRODUCE_EDIT_BTN:
        {
            if ( dwMsg & UIMSG_RNBUTTON_CLICK )
            {
                m_pClubIntroduceEdit->SetVisibleSingle( TRUE );
            }
        }
        break;
    };
}

void CWbClubManagementTab::SetVisibleSingle( BOOL bVisible )
{
    if ( bVisible == TRUE && m_pGaeaClient->GetMyClub()->IsValid() )
    {
        ShowNickName();
        UpdateAutoBanRollOver();
        ShowClubMark();
        //CheckClubPublic();
        SetAutoban();
        ShowButtons();
        ClubUpdateNewbieNotice();
    }

    CUIGroup::SetVisibleSingle( bVisible );
}

void CWbClubManagementTab::ShowNickName()
{
    m_pNickNameText->ClearText();

    std::string& strNickName = m_pGaeaClient->GetClubNickName();
    m_pNickNameText->AddTextNoSplit( strNickName.c_str(), NS_UITEXTCOLOR::WHITE );
}

void CWbClubManagementTab::UpdateAutoBanRollOver()
{
    m_pAutoBanGradeRollOver->ClearText();
    size_t nGradeSize = m_pGaeaClient->GetClubGradeSize();
    for ( size_t i = 0; i < nGradeSize; i++ )
    {
        std::string& strGradeName = m_pGaeaClient->GetClubGradeName( i );
        int nIndex = m_pAutoBanGradeRollOver->AddText( strGradeName.c_str(), true, true, NS_UITEXTCOLOR::WHITE );
    }

    m_pAutoBanDayRollOver->ClearText();
    for ( int i = 0; i < nMAX_AUTO_BAN_DAY; i++ )
    {
        std::string strDay = sc::string::format( "%1%", (i+1) );
        int nIndex = m_pAutoBanDayRollOver->AddText( strDay.c_str(), true, true, NS_UITEXTCOLOR::WHITE );
    }
}

void CWbClubManagementTab::ShowClubMark()
{
    GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
    if ( !pMyClub )
        return;

    DWORD dwClubDbNum = pMyClub->DbNum();
    DWORD dwClubMarkVer = pMyClub->m_dwMarkVER;

    const clubmark::DXDATA& ClubMkark = m_pGaeaClient->GetClubMark( dwClubDbNum, dwClubMarkVer );

    m_pClubMark->SetTexturePos( 0, ClubMkark.vTex_1_LU );
    m_pClubMark->SetTexturePos( 1, ClubMkark.vTex_2_RU );
    m_pClubMark->SetTexturePos( 3, ClubMkark.vTex_3_LD );
    m_pClubMark->SetTexturePos( 2, ClubMkark.vTex_4_RD );
    m_pClubMark->SetTexture( ClubMkark.pddsTexture );
}

//void CWbClubManagementTab::CheckClubPublic()
//{
//    // Get club public state.
//    if ( m_pGaeaClient->GetMyClub()->IsPublicForNewbie() )
//        m_pCheckPublicToNewcomer->SetCheckOn();
//    else
//        m_pCheckPublicToNewcomer->SetCheckOff();
//
//    if ( m_pGaeaClient->GetMyClub()->IsPublicForOtherClubMember() )
//        m_pCheckPublicToOther->SetCheckOn();
//    else
//        m_pCheckPublicToOther->SetCheckOff();
//}

void CWbClubManagementTab::SetAutoban()
{
    const club::AUTO_KICK_OUT& KickOut = m_pGaeaClient->GetMyClub()->GetAutoKickOut();

    size_t GrandeIndex = KickOut.m_GradeIndex;
    if ( GrandeIndex >= 0 && GrandeIndex < m_pGaeaClient->GetClubGradeSize() )
    {
        m_nSelectGradeIndex = static_cast<int>(GrandeIndex);

        m_pAutoBanGradeText->SetText( m_pGaeaClient->GetClubGradeName( GrandeIndex ).c_str(),
        NS_UITEXTCOLOR::WHITE );
    }

    WORD wDay = KickOut.m_Day;
    if ( wDay >=1 && wDay <= nMAX_AUTO_BAN_DAY )
    {
        m_nSelectDayIndex = static_cast<int>(wDay-1);

        std::string strDay = sc::string::format( "%1%", wDay );
        m_pAutoBanDayText->SetText( strDay.c_str(), NS_UITEXTCOLOR::WHITE );
    }

    if ( KickOut.m_Use )
    {
        m_pCheckAutoBan->SetCheckOn();
        EnableAutoban();
    }
    else
    {
        m_pCheckAutoBan->SetCheckOff();
        DisableAutoban();
    }
}

void CWbClubManagementTab::ShowButtons()
{
    DWORD dwCharDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();
	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
    if ( pMyClub->HaveAuthChangeAuthority( dwCharDbNum ) )
    {
        m_pGradeEditBtn->SetVisibleSingle( TRUE );
        m_pAuthorityEditBtn->SetVisibleSingle( TRUE );
        //m_pPublicSaveBtn->SetVisibleSingle( TRUE );
    }
    else
    {
        m_pGradeEditBtn->SetVisibleSingle( FALSE );
        m_pAuthorityEditBtn->SetVisibleSingle( FALSE );
        //m_pPublicSaveBtn->SetVisibleSingle( FALSE );
    }

    if ( pMyClub->HaveMemberKickAuthority( dwCharDbNum ) )
        m_pAutoBanSaveBtn->SetVisibleSingle( TRUE );
    else
         m_pAutoBanSaveBtn->SetVisibleSingle( FALSE );

    if ( pMyClub->IsMaster( dwCharDbNum ) )
        m_pBreakUpClubBtn->SetVisibleSingle( TRUE );
    else
        m_pBreakUpClubBtn->SetVisibleSingle( FALSE );

	if ( pMyClub->IsRegDissolution() )
	{
		ChangeDissolutionTime();
		m_pBreakUpClubBtn->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_17, NS_UITEXTCOLOR::WHITE );
	}
	else
	{
		m_pBreakUpClubBtn->SetText( ID2WORD_WB_CLUB_WINDOW_MANAGEMENT_TAB_16, NS_UITEXTCOLOR::WHITE );
		m_pBreakUpClubText->SetVisibleSingle( FALSE );
	}

}


//void CWbClubManagementTab::SavePublic()
//{
//    bool bPublicToNewcomer = m_pCheckPublicToNewcomer->IsChecked();
//    bool bPublicToOther = m_pCheckPublicToOther->IsChecked();
//
//    // Save club public state.
//    DWORD dwPublicFlag = 0;
//    if ( bPublicToNewcomer )
//        dwPublicFlag |= club::PUBLIC_FOR_NEWBIE;
//    if ( bPublicToOther )
//        dwPublicFlag |= club::PUBLIC_FOR_OTHER_CLUB_MEMBER;
//
//    m_pGaeaClient->ClubPublicTypeReq( dwPublicFlag );
//}

void CWbClubManagementTab::SaveAutoBan()
{
    if ( m_pCheckAutoBan->IsChecked() &&
        (m_nSelectGradeIndex < 0 || m_nSelectDayIndex < 0) )
        return;

    // TODO : Save auto ban state.
    size_t GradeIndex = static_cast<size_t>(m_nSelectGradeIndex);
    WORD wDay = static_cast<WORD>(m_nSelectDayIndex) + 1;

    m_pGaeaClient->ClubAutoKickReq( m_pCheckAutoBan->IsChecked(), GradeIndex, wDay );
}

void CWbClubManagementTab::LeaveClub()
{
    // TODO : Leave club.
    GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
    if ( !pMyClub )
        return;

    //	길드원이 아닌 경우
    if (!pMyClub->IsValid())
        return;

    m_pInterface->DoModal( ID2GAMEINTEXT("CLUB_SELFQUIT"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_SELFQUIT );
}

void CWbClubManagementTab::ChangeClubMark()
{
    GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
    if ( !pMyClub )
        return;

    if (!pMyClub->IsValid())
        return;

    DWORD dwCharDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();
    if (!pMyClub->HaveMarkChangeAuthority(dwCharDbNum))
        return;

    m_pInterface->DoModal( ID2GAMEINTEXT("CLUB_CHANGE_MARK"),
        UI::MODAL_INPUT, UI::MODAL_FILE_NAME, UI::MODAL_CLUB_CHANGE_MARK );
}

void CWbClubManagementTab::ChangeDissolutionTime()
{
	CTime tEstimate;
	if ( m_pGaeaClient->GetMyClub()->GetDissolution( tEstimate ) )
	{
		std::string strText = sc::string::format( ID2INTEXT_WB_CLUB_WINDOW_MANAGEMENT_BREAK_UP_ESTIMATED_TIME, 
			tEstimate.GetYear(), tEstimate.GetMonth(), tEstimate.GetDay(), tEstimate.GetHour(), tEstimate.GetMinute() );
		m_pBreakUpClubText->SetText( strText.c_str(), NS_UITEXTCOLOR::RED );
		m_pBreakUpClubText->SetVisibleSingle( TRUE );
	}
}

void CWbClubManagementTab::EnableAutoban()
{
    m_pAutoBanGradeBtn->SetVisibleSingle( TRUE );
    m_pAutoBanDayBtn->SetVisibleSingle( TRUE );

    m_pAutoBanGradeText->SetDiffuseAlpha( 255 );
    m_pAutoBanDayText->SetDiffuseAlpha( 255 );
}

void CWbClubManagementTab::DisableAutoban()
{
    m_pAutoBanGradeBtn->SetVisibleSingle( FALSE );
    m_pAutoBanDayBtn->SetVisibleSingle( FALSE );

    m_pAutoBanGradeText->SetDiffuseAlpha( 70 );
    m_pAutoBanDayText->SetDiffuseAlpha( 70 );

    m_pAutoBanGradeRollOver->SetVisibleSingle( FALSE );
    m_pAutoBanDayRollOver->SetVisibleSingle( FALSE );
}

void CWbClubManagementTab::ClubUpdateNewbieNotice()
{
    //std::string& strNewbieNotice = m_pGaeaClient->ClubNewbieNotice();
    //m_pIntroduceText->SetText( strNewbieNotice.c_str(), NS_UITEXTCOLOR::WHITE );
}
