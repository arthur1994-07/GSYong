#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"

#include "../Util/CheckBox.h"
#include "../Util/PopupMenuComponent.h"
#include "./WbClubInfoField.h"
#include "./WbClubBasicInfoGrade.h"
#include "./WbClubBasicInfoCommand.h"
#include "./WbClubBasicInfoNoticeEdit.h"
#include "./WbClubBasicInfoRelation.h"
#include "./WbClubBasicInfoTab.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

__time64_t CWbClubBasicInfoTab::tDEFAULT_NOTICE_DATA = 2649661;

namespace
{
    const char* ID2GAMEWORD_CLASS_FIELD = "직업";
    const char* ID2GAMEWORD_LEVEL_FIELD = "레벨";
    const char* ID2GAMEWORD_NAME_FIELD = "이름";
    const char* ID2GAMEWORD_CHANNEL_FIELD = "채널";
    const char* ID2GAMEWORD_POSITION_FIELD = "위치";
    const char* ID2GAMEWORD_GRADE_FIELD = "계급";

    const char* ID2GAMEWORD_BASIC_INFO_BTN_0 = "추방";
    const char* ID2GAMEWORD_BASIC_INFO_BTN_1 = "마스터 위임";
    const char* ID2GAMEWORD_BASIC_INFO_BTN_2 = "CD 인증권한 부여";
    const char* ID2GAMEWORD_BASIC_INFO_BTN_3 = "계급 변경";
    const char* ID2GAMEWORD_BASIC_INFO_BTN_4 = "CD 인증권한 해제";

    const char* ID2GAMEWORD_BASIC_INFO_MASTER_MARK_0 = "[M]";
    const char* ID2GAMEWORD_BASIC_INFO_MASTER_MARK_1 = "[S]";
    const char* ID2GAMEWORD_BASIC_INFO_MASTER_MARK_2 = "[CD]";
    const char* ID2GAMEWORD_BASIC_INFO_MASTER_MARK_3 = "마스터";
    const char* ID2GAMEWORD_BASIC_INFO_MASTER_MARK_4 = "부마스터";

	const char* ID2GAMEWORD_BASIC_INFO_CLASS_FIGHTER = "F";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_ARMS = "K";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_ARCHER = "A";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_SPIRIT = "M";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_EXTREME = "E";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_SCIENTIST = "S";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_ASSASSIN = "N";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_TRICKER = "T";
	const char* ID2GAMEWORD_BASIC_INFO_CLASS_ETC = "T";
	const int	CLASS_MAX_NUM = 9;

    const char* ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_0 = "EDIT";
}

CWbClubBasicInfoTab::CWbClubBasicInfoTab( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pListBack(NULL)
    , m_pClassField(NULL)
    , m_pLevelField(NULL)
    , m_pNameField(NULL)
    , m_pChannelField(NULL)
    , m_pPositionField(NULL)
    , m_pGradeField(NULL)
    , m_pOnlyOnlineCheck(NULL)
    , m_pOnlyOnlineText(NULL)
    , m_pOnOfflineText(NULL)
    , m_pBanBtn(NULL)
    , m_pDelegateBtn(NULL)
    , m_pCDAuthorizeBtn(NULL)
    , m_pChangeGradeBtn(NULL)
    , m_pSetGradePopup(NULL)
    , m_pCommandPopup(NULL)
    , m_pNoticeTitle(NULL)
    , m_pNoticeDate(NULL)
    , m_pNoticeBody(NULL)
    , m_pNoticeScrollBar(NULL)
    , m_pNoticeEditBtn(NULL)
    , m_pNoticeEdit(NULL)
    , m_pActivityTitle(NULL)
    , m_pActivityBody(NULL)
    , m_pActivityScrollBar(NULL)
    , m_pRelation(NULL)
    , m_nSelectIndex(-1)
    , m_bShowCommandPopup(false)
{
    ID2GAMEWORD_CLASS_FIELD = ID2GAMEWORD("WB_CLUB_BASIC_INFO_TAB_FIELD", 0);
    ID2GAMEWORD_LEVEL_FIELD = ID2GAMEWORD("WB_CLUB_BASIC_INFO_TAB_FIELD", 1);
    ID2GAMEWORD_NAME_FIELD = ID2GAMEWORD("WB_CLUB_BASIC_INFO_TAB_FIELD", 2);
    ID2GAMEWORD_CHANNEL_FIELD = ID2GAMEWORD("WB_CLUB_BASIC_INFO_TAB_FIELD", 3);
    ID2GAMEWORD_POSITION_FIELD = ID2GAMEWORD("WB_CLUB_BASIC_INFO_TAB_FIELD", 4);
    ID2GAMEWORD_GRADE_FIELD = ID2GAMEWORD("WB_CLUB_BASIC_INFO_TAB_FIELD", 5);

    ID2GAMEWORD_BASIC_INFO_BTN_0 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 0);
    ID2GAMEWORD_BASIC_INFO_BTN_1 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 1);
    ID2GAMEWORD_BASIC_INFO_BTN_2 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 2);
    ID2GAMEWORD_BASIC_INFO_BTN_3 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 3);
    ID2GAMEWORD_BASIC_INFO_BTN_4 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 4);

    ID2GAMEWORD_BASIC_INFO_MASTER_MARK_0 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 0);
    ID2GAMEWORD_BASIC_INFO_MASTER_MARK_1 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 1);
    ID2GAMEWORD_BASIC_INFO_MASTER_MARK_2 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 2);
    ID2GAMEWORD_BASIC_INFO_MASTER_MARK_3 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 3);
    ID2GAMEWORD_BASIC_INFO_MASTER_MARK_4 = ID2GAMEWORD("WB_CLUB_BASIC_INFO_MASTER_MARK", 4);

	static CString strClass[GLCL_NUM_CLASS];
	for( int i=0; i<GLCL_NUM_CLASS; ++i )
	{
		strClass[i] = ID2GAMEWORD("WB_CLUB_BASIC_INFO_CLASS", i);
		StrCutByte( strClass[i], strClass[i], 2 );
	}

	ID2GAMEWORD_BASIC_INFO_CLASS_FIGHTER	= strClass[0];
	ID2GAMEWORD_BASIC_INFO_CLASS_ARMS		= strClass[1];
	ID2GAMEWORD_BASIC_INFO_CLASS_ARCHER		= strClass[2];
	ID2GAMEWORD_BASIC_INFO_CLASS_SPIRIT		= strClass[3];
	ID2GAMEWORD_BASIC_INFO_CLASS_EXTREME	= strClass[4];
	ID2GAMEWORD_BASIC_INFO_CLASS_SCIENTIST	= strClass[5];
	ID2GAMEWORD_BASIC_INFO_CLASS_ASSASSIN	= strClass[6];
	ID2GAMEWORD_BASIC_INFO_CLASS_TRICKER	= strClass[7];
	ID2GAMEWORD_BASIC_INFO_CLASS_ETC			= strClass[8];

    ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_0 = ID2GAMEWORD("WB_CLUB_WINDOW_INFO_NOTICE_EDIT", 0);

    m_vecMembers.clear();
}

void CWbClubBasicInfoTab::StrCutByte( CString& strInput, CString& strOutput, BYTE ByteLenth )
{
	if( strInput.GetLength() == 0 )
		return;

	if( strInput.GetLength() < ByteLenth )
		return;

	CString strData = "";
	LPTSTR strTemp = (LPTSTR)(LPCTSTR)strInput;
	CHAR chStr[3];
	memset( chStr, 0, sizeof(chStr) );

	for( BYTE i=0; i<ByteLenth; )
	{
		if( (*strTemp & 0x80) )
		{
			if( (i+2) <= ByteLenth )
			{
				memcpy(chStr, strTemp, 2);
				strTemp = AnsiNext(strTemp);
				strData += chStr;
				memset( chStr, 0, sizeof(chStr) );
				i += 2;
			}
			else
			{
				break;
			}
		}
		else
		{
			if( (i+1) <= ByteLenth )
			{
				memcpy(chStr, strTemp, 1);
				strTemp = AnsiNext(strTemp);
				strData += chStr;
				memset( chStr, 0, sizeof(chStr) );
				i += 1;
			}
			else
			{
				break;
			}
		}
	}
	strOutput = strData;
}

CWbClubBasicInfoTab::~CWbClubBasicInfoTab()
{

}

void CWbClubBasicInfoTab::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
        m_pListBack = pLinBoxSmart;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_ACTIVITY_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CWbClubInfoField* pClubInfoField = new CWbClubInfoField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubInfoField->CreateSub( this, "WB_BASIC_INFO_TAB_CLASS" );
        pClubInfoField->CreateSubControl( "WB_BASIC_INFO_TAB_CLASS_BACK",
            "WB_BASIC_INFO_TAB_CLASS_TEXT",
            "WB_BASIC_INFO_TAB_CLASS_ORDER",
            "WB_BASIC_INFO_TAB_CLASS_LIST",
            TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
        RegisterControl( pClubInfoField );
        m_pClassField = pClubInfoField;
        m_pClassField->SetFiledText( ID2GAMEWORD_CLASS_FIELD );
        m_pClassField->ResetOrder();

        pClubInfoField = new CWbClubInfoField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubInfoField->CreateSub( this, "WB_BASIC_INFO_TAB_LEVEL"  );
        pClubInfoField->CreateSubControl( "WB_BASIC_INFO_TAB_LEVEL_BACK",
            "WB_BASIC_INFO_TAB_LEVEL_TEXT",
            "WB_BASIC_INFO_TAB_LEVEL_ORDER",
            "WB_BASIC_INFO_TAB_LEVEL_LIST",
            TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
        RegisterControl( pClubInfoField );
        m_pLevelField = pClubInfoField;
        m_pLevelField->SetFiledText( ID2GAMEWORD_LEVEL_FIELD );

        pClubInfoField = new CWbClubInfoField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubInfoField->CreateSub( this, "WB_BASIC_INFO_TAB_NAME"  );
        pClubInfoField->CreateSubControl( "WB_BASIC_INFO_TAB_NAME_BACK",
            "WB_BASIC_INFO_TAB_NAME_TEXT",
            "WB_BASIC_INFO_TAB_NAME_ORDER",
            "WB_BASIC_INFO_TAB_NAME_LIST",
            TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pClubInfoField );
        m_pNameField = pClubInfoField;
        m_pNameField->SetFiledText( ID2GAMEWORD_NAME_FIELD );

        pClubInfoField = new CWbClubInfoField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubInfoField->CreateSub( this, "WB_BASIC_INFO_TAB_CHANNEL"  );
        pClubInfoField->CreateSubControl( "WB_BASIC_INFO_TAB_CHANNEL_BACK",
            "WB_BASIC_INFO_TAB_CHANNEL_TEXT",
            "WB_BASIC_INFO_TAB_CHANNEL_ORDER",
            "WB_BASIC_INFO_TAB_CHANNEL_LIST",
            TEXT_ALIGN_CENTER_X | TEXT_ALIGN_TOP );
        RegisterControl( pClubInfoField );
        m_pChannelField = pClubInfoField;
        m_pChannelField->SetFiledText( ID2GAMEWORD_CHANNEL_FIELD );
        m_pChannelField->ResetOrder();

        pClubInfoField = new CWbClubInfoField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubInfoField->CreateSub( this, "WB_BASIC_INFO_TAB_POSITION"  );
        pClubInfoField->CreateSubControl( "WB_BASIC_INFO_TAB_POSITION_BACK",
            "WB_BASIC_INFO_TAB_POSITION_TEXT",
            "WB_BASIC_INFO_TAB_POSITION_ORDER",
            "WB_BASIC_INFO_TAB_POSITION_LIST",
            TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pClubInfoField );
        m_pPositionField = pClubInfoField;
        m_pPositionField->SetFiledText( ID2GAMEWORD_POSITION_FIELD );
        m_pPositionField->ResetOrder();

        pClubInfoField = new CWbClubInfoField( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pClubInfoField->CreateSub( this, "WB_BASIC_INFO_TAB_GRADE"  );
        pClubInfoField->CreateSubControl( "WB_BASIC_INFO_TAB_GRADE_BACK",
            "WB_BASIC_INFO_TAB_GRADE_TEXT",
            "WB_BASIC_INFO_TAB_GRADE_ORDER",
            "WB_BASIC_INFO_TAB_GRADE_LIST",
            TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pClubInfoField );
        m_pGradeField = pClubInfoField;
        m_pGradeField->SetFiledText( ID2GAMEWORD_GRADE_FIELD );
    }

    {
        CBasicLineBoxSmart* pLineBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLineBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_0_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLineBoxSmart );

        pLineBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLineBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_1_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLineBoxSmart );

        pLineBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLineBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_2_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLineBoxSmart );

        pLineBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLineBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_3_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLineBoxSmart );

        pLineBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLineBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_4_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLineBoxSmart );
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_CLUB_WINDOW_TAB_BASIC_INFO_BACK" );
        RegisterControl( pControl );

        //	스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "WB_CLUB_WINDOW_TAB_BASIC_INFO_WINDOW_SCROLLBAR" );
        pScrollBar->GetThumbFrame()->SetState( 1, 0 );
        pScrollBar->GetThumbFrame()->SetScrollParent( pControl );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 2 );
        RegisterControl( pScrollBar );
        m_pListScroll = pScrollBar;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_LEVEL_BOX", UI_FLAG_DEFAULT, LEVEL_FIELD );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_NAME_BOX", UI_FLAG_DEFAULT, NAME_FIELD );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_GRADE_BOX", UI_FLAG_DEFAULT, GRADE_FIELD );
        RegisterControl( pControl );
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_CLASS_LIST_BOX", UI_FLAG_DEFAULT, CLASS_FIELD_LIST );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_LEVEL_LIST_BOX", UI_FLAG_DEFAULT, LEVEL_FIELD_LIST );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_NAME_LIST_BOX", UI_FLAG_DEFAULT, NAME_FIELD_LIST );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_CHANNEL_LIST_BOX", UI_FLAG_DEFAULT, CHANNEL_FIELD_LIST );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_POSITION_LIST_BOX", UI_FLAG_DEFAULT, POSITON_FIELD_LIST );
        RegisterControl( pControl );

        pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_BASIC_INFO_TAB_GRADE_LIST_BOX", UI_FLAG_DEFAULT, GRADE_FIELD_LIST );
        RegisterControl( pControl );
    }

    {
        CCheckBox* pCheckBox = new CCheckBox( m_pEngineDevice );
        pCheckBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_ONLY_ONLINE_CHECK_BOX", UI_FLAG_DEFAULT, ONLY_ONLINE_CHECK );
        pCheckBox->CreateSubControl( "PRODUCT_WINDOW_LIST_CHECK", "PRODUCT_WINDOW_LIST_UNCHECK" );
        RegisterControl( pCheckBox );
        m_pOnlyOnlineCheck = pCheckBox;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_ONLY_ONLINE_TEXT" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pOnlyOnlineText = pTextBox;
        m_pOnlyOnlineText->SetText( ID2GAMEINTEXT("WB_CLUB_WINDOW_BASIC_INFO_ONLY_ONLINE_TEXT"),
            NS_UITEXTCOLOR::WHITE );
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_ONOFFLINE_TEXT" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pOnOfflineText = pTextBox;
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
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_BAN_BTN", UI_FLAG_DEFAULT, BAN_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pBanBtn = pRnButton;

        arg.text = ID2GAMEWORD_BASIC_INFO_BTN_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_DELEGATE_BTN", UI_FLAG_DEFAULT, DELEGATE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pDelegateBtn = pRnButton;

        arg.text = ID2GAMEWORD_BASIC_INFO_BTN_2;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_CDAUTHORIZE_BTN", UI_FLAG_DEFAULT, CD_AUTHORIZE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pCDAuthorizeBtn = pRnButton;

        arg.text = ID2GAMEWORD_BASIC_INFO_BTN_3;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_CHANGEGRADE_BTN", UI_FLAG_DEFAULT, CHANGE_GRADE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pChangeGradeBtn = pRnButton;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_TITLE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_LIST_FIELD_LINE" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_TITLE" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pNoticeTitle = pTextBox;
        m_pNoticeTitle->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_INFO_RIGHT_SIDE", 0), NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_DATE" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_RIGHT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pNoticeDate = pTextBox;

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_BODY" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        m_pNoticeBody = pTextBox;

        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_SCROLLBAR" );
        pScrollBar->GetThumbFrame()->SetState( 1, 0 );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pNoticeBody );
        RegisterControl( pScrollBar );
        m_pNoticeScrollBar = pScrollBar;

        RnButton* pRnButton = NULL;

        RnButton::CreateArg arg;
        //////////////////////////////////////////////////////////////////////////
        arg.defaultTextureKey = "RNBUTTON_DEFAULT_TEXTURE";
        arg.mouseOverTextureKey = "RNBUTTON_OVER_TEXTURE";
        arg.mouseClickTextureKey = "RNBUTTON_CLICK_TEXTURE";

        arg.pFont = pFont8Shadow;
        arg.dwColor = NS_UITEXTCOLOR::WHITE;
        //////////////////////////////////////////////////////////////////////////

        arg.text = ID2GAMEWORD_BASIC_INFO_NOTICE_EDIT_0;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT_BTN", UI_FLAG_DEFAULT, NOTICE_EDIT_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pNoticeEditBtn = pRnButton;
    }

    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_ACTIVITY_TITLE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_LIST_FIELD_LINE" );
        RegisterControl( pLinBoxSmart );

        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
        CD3DFontPar* pFont8Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_ACTIVITY_TITLE" );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
        RegisterControl( pTextBox );
        m_pActivityTitle = pTextBox;
        m_pActivityTitle->SetText( ID2GAMEWORD("WB_CLUB_WINDOW_INFO_RIGHT_SIDE", 1), NS_UITEXTCOLOR::WHITE );

        pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_ACTIVITY_BODY" );
        pTextBox->SetFont( pFont8Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        pTextBox->SetPartInterval( 10.0f );
        RegisterControl( pTextBox );
        m_pActivityBody = pTextBox;

        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "WB_CLUB_WINDOW_BASIC_INFO_ACTIVITY_BODY_SCROLL" );
        pScrollBar->GetThumbFrame()->SetState( 1, 0 );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pActivityBody );
        RegisterControl( pScrollBar );
        m_pActivityScrollBar = pScrollBar;
    }

    {
        CWbClubInfoNoticeEdit* pNoticeEdit = new CWbClubInfoNoticeEdit( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pNoticeEdit->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_NOTICE_EDIT", UI_FLAG_DEFAULT, NOTICE_EDIT );
        pNoticeEdit->CreateSubControl();
        RegisterControl( pNoticeEdit );
        pNoticeEdit->SetClubNoticeType();
        pNoticeEdit->SetLimitInput( EMCLUB_NOTICE_LEN - 1 );
        pNoticeEdit->SetVisibleSingle( FALSE );
        m_pNoticeEdit = pNoticeEdit;
    }

    {
        CWbClubBasicInfoRelation* pRelation = new CWbClubBasicInfoRelation( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pRelation->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BACK", UI_FLAG_DEFAULT, RELATION );
        pRelation->CreateSubControl();
        RegisterControl( pRelation );
        m_pRelation = pRelation;
    }

    {
        CWbClubBasicInfoGrade* pBasicInfoGrade = new CWbClubBasicInfoGrade( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pBasicInfoGrade->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_GRADE_POPUP", UI_FLAG_DEFAULT, SET_GRADE_POPUP );
        pBasicInfoGrade->CreateSubControl();
        pBasicInfoGrade->SetVisibleSingle( FALSE );
        RegisterControl( pBasicInfoGrade );
        m_pSetGradePopup = pBasicInfoGrade;
    }

    {
        CWbClubBasicInfoCommand* pBasicInfoCommand = new CWbClubBasicInfoCommand( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pBasicInfoCommand->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_COMMAND_POPUP", UI_FLAG_DEFAULT, COMMAND_POPUP );
        pBasicInfoCommand->CreateSubControl();
        pBasicInfoCommand->SetVisibleSingle( FALSE );
        RegisterControl( pBasicInfoCommand );
        m_pCommandPopup = pBasicInfoCommand;
    }

    SetClubListOrder( club::ORDER_CHA_LEVEL, club::SQL_DESC );
}

void CWbClubBasicInfoTab::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_pSetGradePopup->IsVisible() )
        return;

    if ( m_bShowCommandPopup )
    {
        m_bShowCommandPopup = false;

        const UIRECT& rcListBackGP = m_pListBack->GetGlobalPos();

        if ( x >= rcListBackGP.left && x <= rcListBackGP.right &&
            y >= m_rcSelectPart.top && y <= m_rcSelectPart.bottom )
        {
            popupmenu::SP_POPUP_MENU PopupMenu( new popupmenu::PopupMenuComposite( "", false) );

            PopupMenu->addComponent( popupmenu::CreatePopupMenuItem( std::string(ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 0)), boost::bind( &CWbClubBasicInfoTab::KickMember, this, _1 )) );
            PopupMenu->addComponent( popupmenu::CreatePopupMenuItem( std::string(ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 1)), boost::bind( &CWbClubBasicInfoTab::DelegateMaster, this, _1 )) );
            if ( IsCDAuthorizeToMaster() )
                PopupMenu->addComponent( popupmenu::CreatePopupMenuItem( std::string(ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 2)), boost::bind( &CWbClubBasicInfoTab::AuthorizeCdCertifier, this, _1 )) );
            else
                PopupMenu->addComponent( popupmenu::CreatePopupMenuItem( std::string(ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 4)), boost::bind( &CWbClubBasicInfoTab::AuthorizeCdCertifier, this, _1 )) );
            PopupMenu->addComponent( popupmenu::CreatePopupMenuItem( std::string(ID2GAMEWORD("WB_CLUB_BASIC_INFO_BTN", 3)), boost::bind( &CWbClubBasicInfoTab::ChangeGrade, this, _1 )) );

            m_pInterface->OpenPopupMenu( PopupMenu );
        }
    }

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pListScroll->GetThumbFrame();

        const int nTotalLine = static_cast<int>( m_vecMembers.size() );
        const int nLinePerOneView = m_pLevelField->GetVisibleListLine();

        pThumbFrame->SetState( nTotalLine, nLinePerOneView );
        if ( nLinePerOneView < nTotalLine )
        {
            const int nMovableLine = nTotalLine - nLinePerOneView;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = static_cast<int>(floor(fPercent * nMovableLine));

            m_pClassField->SetCurListLine( nPos );
            m_pLevelField->SetCurListLine( nPos );
            m_pNameField->SetCurListLine( nPos );
            m_pChannelField->SetCurListLine( nPos );
            m_pPositionField->SetCurListLine( nPos );
            m_pGradeField->SetCurListLine( nPos );
        }
    }

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pNoticeScrollBar->GetThumbFrame();

        const int nTotalLine = m_pNoticeBody->GetTotalLine();
        const int nLinePerOneView = m_pNoticeBody->GetVisibleLine();

        pThumbFrame->SetState( nTotalLine, nLinePerOneView );
        if ( nLinePerOneView < nTotalLine )
        {
            const int nMovableLine = nTotalLine - nLinePerOneView;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = (int)floor(fPercent * nMovableLine);
            m_pNoticeBody->SetCurLine( nPos );			
        }
    }

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pActivityScrollBar->GetThumbFrame();

        const int nTotalLine = m_pActivityBody->GetTotalLine();
        const int nLinePerOneView = ACTIVITY_VISIBLE_LINE;

        pThumbFrame->SetState( nTotalLine, nLinePerOneView );
        if ( nLinePerOneView < nTotalLine )
        {
            const int nMovableLine = nTotalLine - nLinePerOneView;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = (int)floor(fPercent * nMovableLine);
            m_pActivityBody->SetCurLine( nPos );			
        }
    }
}

void CWbClubBasicInfoTab::TranslateUIMessage( UIGUID controlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( controlID, dwMsg );

    if ( m_pSetGradePopup->IsVisible() ||
        m_pNoticeEdit->IsVisible() )
        return;

    if ( controlID == COMMAND_POPUP )
    {
        if ( !CHECK_MOUSE_IN( dwMsg ) )
        {
            if ( CHECK_LB_UP_LIKE( dwMsg ) )
                m_pCommandPopup->SetVisibleSingle( FALSE );
        }
    }

    if ( m_pCommandPopup->IsVisible() )
        return;

    switch ( controlID )
    {
    case NOTICE_EDIT_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                m_pNoticeEdit->SetVisibleSingle( TRUE );
            }
        }
        break;

    case BAN_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                KickMember();
            }
        }
        break;

    case DELEGATE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                DelegateMaster();
            }
        }
        break;

    case CD_AUTHORIZE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                AuthorizeCdCertifier();
            }
        }
        break;

    case CHANGE_GRADE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                ChangeGrade();
            }
        }
        break;

    case ONLY_ONLINE_CHECK:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                ShowClubMember();
            }
        }
        break;

    case LEVEL_FIELD:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                club::ALIGN_ORDER_DES_ASC Order = m_pLevelField->GetFieldOrder();

                if ( Order == club::SQL_ASC )
                    Order = club::SQL_DESC;
                else
                    Order = club::SQL_ASC;

                m_pLevelField->SetFieldOrder( Order );

                SetClubListOrder( club::ORDER_CHA_LEVEL, Order );
                ShowClubMember();
            }
        }
        break;

    case NAME_FIELD:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                club::ALIGN_ORDER_DES_ASC Order = m_pNameField->GetFieldOrder();

                if ( Order == club::SQL_ASC )
                    Order = club::SQL_DESC;
                else
                    Order = club::SQL_ASC;

                m_pNameField->SetFieldOrder( Order );

                SetClubListOrder( club::ORDER_CHA_NAME, Order );
                ShowClubMember();
            }
        }
        break;

    case GRADE_FIELD:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                club::ALIGN_ORDER_DES_ASC Order = m_pGradeField->GetFieldOrder();

                if ( Order == club::SQL_ASC )
                    Order = club::SQL_DESC;
                else
                    Order = club::SQL_ASC;

                m_pGradeField->SetFieldOrder( Order );

                SetClubListOrder( club::ORDER_FLAG, Order );
                ShowClubMember();
            }
        }
        break;

    case CLASS_FIELD_LIST:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                SelectMember( m_pClassField->GetSelectPos() );

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                if ( IsSelectedIndex() )
                {
                    DWORD dwCharDbNum = GetSelectMember().m_ChaDbNum;
                    const char* strCharName = GetSelectMember().GetName();
                    DWORD dwGrade = GetSelectMember().GetGrade();

                    if ( m_pCommandPopup->SetCommandData( dwCharDbNum, strCharName, dwGrade ) )
                    {
                        m_bShowCommandPopup = true;
                        m_pClassField->GetSelectPart( m_nSelectIndex, m_rcSelectPart );
                    }
                    else
                        m_pCommandPopup->SetVisibleSingle( FALSE );
                }
            }
        }
        break;

    case LEVEL_FIELD_LIST:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                SelectMember( m_pLevelField->GetSelectPos() );

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                if ( IsSelectedIndex() )
                {
                    DWORD dwCharDbNum = GetSelectMember().m_ChaDbNum;
                    const char* strCharName = GetSelectMember().GetName();
                    DWORD dwGrade = GetSelectMember().GetGrade();

                    if ( m_pCommandPopup->SetCommandData( dwCharDbNum, strCharName, dwGrade ) )
                    {
                        m_bShowCommandPopup = true;
                        m_pLevelField->GetSelectPart( m_nSelectIndex, m_rcSelectPart );
                    }
                    else
                        m_pCommandPopup->SetVisibleSingle( FALSE );
                }
            }
        }
        break;

    case NAME_FIELD_LIST:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                SelectMember( m_pNameField->GetSelectPos() );

			if ( CHECK_MOUSEIN_LBDUPLIKE( dwMsg ) )
			{				
				if( IsSelectedIndex() )
				{
					const char* strCharName = GetSelectMember().GetName();
					m_pInterface->GetBasicChatWindow()->BeginPrivateChat( strCharName );
				}
			}

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                if ( IsSelectedIndex() )
                {
                    DWORD dwCharDbNum = GetSelectMember().m_ChaDbNum;
                    const char* strCharName = GetSelectMember().GetName();
                    DWORD dwGrade = GetSelectMember().GetGrade();

                    if ( m_pCommandPopup->SetCommandData( dwCharDbNum, strCharName, dwGrade ) )
                    {
                        m_bShowCommandPopup = true;
                        m_pNameField->GetSelectPart( m_nSelectIndex, m_rcSelectPart );
                    }
                    else
                        m_pCommandPopup->SetVisibleSingle( FALSE );
                }
            }
        }
        break;

    case CHANNEL_FIELD_LIST:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                SelectMember( m_pChannelField->GetSelectPos() );

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                if ( IsSelectedIndex() )
                {
                    DWORD dwCharDbNum = GetSelectMember().m_ChaDbNum;
                    const char* strCharName = GetSelectMember().GetName();
                    DWORD dwGrade = GetSelectMember().GetGrade();

                    if ( m_pCommandPopup->SetCommandData( dwCharDbNum, strCharName, dwGrade ) )
                    {
                        m_bShowCommandPopup = true;
                        m_pChannelField->GetSelectPart( m_nSelectIndex, m_rcSelectPart );
                    }
                    else
                        m_pCommandPopup->SetVisibleSingle( FALSE );
                }
            }
        }
        break;

    case POSITON_FIELD_LIST:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                SelectMember( m_pPositionField->GetSelectPos() );

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                if ( IsSelectedIndex() )
                {
                    DWORD dwCharDbNum = GetSelectMember().m_ChaDbNum;
                    const char* strCharName = GetSelectMember().GetName();
                    DWORD dwGrade = GetSelectMember().GetGrade();

                    if ( m_pCommandPopup->SetCommandData( dwCharDbNum, strCharName, dwGrade ) )
                    {
                        m_bShowCommandPopup = true;
                        m_pPositionField->GetSelectPart( m_nSelectIndex, m_rcSelectPart );
                    }
                    else
                        m_pCommandPopup->SetVisibleSingle( FALSE );
                }
            }
        }
        break;

    case GRADE_FIELD_LIST:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
                SelectMember( m_pGradeField->GetSelectPos() );

            if ( CHECK_MOUSEIN_RBUPLIKE( dwMsg ) )
            {
                if ( IsSelectedIndex() )
                {
                    DWORD dwCharDbNum = GetSelectMember().m_ChaDbNum;
                    const char* strCharName = GetSelectMember().GetName();
                    DWORD dwGrade = GetSelectMember().GetGrade();

                    if ( m_pCommandPopup->SetCommandData( dwCharDbNum, strCharName, dwGrade ) )
                    {
                        m_bShowCommandPopup = true;
                        m_pGradeField->GetSelectPart( m_nSelectIndex, m_rcSelectPart );
                    }
                    else
                        m_pCommandPopup->SetVisibleSingle( FALSE );
                }
            }
        }
        break;
    };
}

void CWbClubBasicInfoTab::SelectMember( int nIndex )
{
    m_nSelectIndex = nIndex;

    m_pClassField->SetSelectPos( m_nSelectIndex );
    m_pLevelField->SetSelectPos( m_nSelectIndex );
    m_pNameField->SetSelectPos( m_nSelectIndex );
    m_pChannelField->SetSelectPos( m_nSelectIndex );
    m_pPositionField->SetSelectPos( m_nSelectIndex );
    m_pGradeField->SetSelectPos( m_nSelectIndex );

    CheckShowButtons();
}

void CWbClubBasicInfoTab::CheckShowButtons()
{
    if ( !IsSelectedIndex() )
        return;

    DWORD dwID = m_pGaeaClient->GetCharacter()->CharDbNum();

    if ( m_pGaeaClient->GetMyClub()->HaveMemberKickAuthority( dwID ) )
        m_pBanBtn->SetVisibleSingle( TRUE );
    else
        m_pBanBtn->SetVisibleSingle( FALSE );

    if ( m_pGaeaClient->GetMyClub()->IsMaster( dwID ) )
    {
        m_pDelegateBtn->SetVisibleSingle( TRUE );
        m_pCDAuthorizeBtn->SetVisibleSingle( TRUE );
    }
    else
    {
        m_pDelegateBtn->SetVisibleSingle( FALSE );
        m_pCDAuthorizeBtn->SetVisibleSingle( FALSE );
    }

    if ( m_pGaeaClient->GetMyClub()->HaveAuthChangeAuthority( dwID ) )
        m_pChangeGradeBtn->SetVisibleSingle( TRUE );
    else
        m_pChangeGradeBtn->SetVisibleSingle( FALSE );
}

void CWbClubBasicInfoTab::SetVisibleSingle( BOOL isVisible )
{
    if ( isVisible == TRUE )
    {
        ShowClubMember();
        ShowClubLog();
        ShowNotice();
        m_pRelation->EventSelectedTap( m_pRelation->GetCurTab() );
    }

    CUIGroup::SetVisibleSingle( isVisible );
}

// 임시해결
HRESULT CWbClubBasicInfoTab::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr( S_OK );
	V_RETURN( CUIGroup::RestoreDeviceObjects( pd3dDevice ) );

	m_pClassField->SetSelectPos( m_nSelectIndex );
	m_pLevelField->SetSelectPos( m_nSelectIndex );
	m_pNameField->SetSelectPos( m_nSelectIndex );
	m_pChannelField->SetSelectPos( m_nSelectIndex );
	m_pPositionField->SetSelectPos( m_nSelectIndex );
	m_pGradeField->SetSelectPos( m_nSelectIndex );

	return S_OK;
}

void CWbClubBasicInfoTab::SetClubListOrder( club::ALIGN_ORDER_TYPE Type, club::ALIGN_ORDER_DES_ASC Order )
{
    if ( m_OrderType != Type )
        Order = club::SQL_ASC;

    m_OrderType = Type;
    m_OrderASC_DSC = Order;

    m_pLevelField->ResetOrder();
    m_pNameField->ResetOrder();
    m_pGradeField->ResetOrder();

    if ( Type == club::ORDER_CHA_LEVEL )
    {
        m_pLevelField->SetFieldOrder( Order );
    }
    else if ( Type == club::ORDER_CHA_NAME )
    {
        m_pNameField->SetFieldOrder( Order );
    }
    else if ( Type == club::ORDER_FLAG )
    {
        m_pGradeField->SetFieldOrder( Order );
    }
}

void CWbClubBasicInfoTab::ShowClubMember()
{
    m_pClassField->ClearList();
    m_pLevelField->ClearList();
    m_pNameField->ClearList();
    m_pChannelField->ClearList();
    m_pPositionField->ClearList();
    m_pGradeField->ClearList();

    GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
    if (!pMyClub->IsValid())
        return;

    std::vector<club::ALIGN_ORDER> MemberAlign;

    club::ALIGN_ORDER Align( m_OrderType, m_OrderASC_DSC );
    MemberAlign.push_back(Align);

    m_vecMembers.clear();
    m_pGaeaClient->GetClubMember(m_vecMembers, MemberAlign, true);
    m_pGaeaClient->GetClubMember(m_vecMembers, MemberAlign, false);

    for (size_t i=0; i<m_vecMembers.size(); ++i)
    {
        club::GLCLUBMEMBER_CLIENT& sClubMember = m_vecMembers[i];

        if ( m_pOnlyOnlineCheck->IsChecked() )
        {
            if ( sClubMember.m_Online )
                AddClubMember( sClubMember );
        }
        else
            AddClubMember( sClubMember );
    }

    m_pClassField->SetCurListLine( 0 );
    m_pLevelField->SetCurListLine( 0 );
    m_pNameField->SetCurListLine( 0 );
    m_pChannelField->SetCurListLine( 0 );
    m_pPositionField->SetCurListLine( 0 );
    m_pGradeField->SetCurListLine( 0 );

	const float fPercent = m_pListScroll->GetThumbFrame()->GetPercent();

	const int nTotal = static_cast<int>( m_vecMembers.size() );
	const int nViewPerPage = m_pClassField->GetVisibleListLine();	
	m_pListScroll->GetThumbFrame()->SetState( nTotal, nViewPerPage );
	m_pListScroll->GetThumbFrame()->SetPercent( fPercent );

    club::MEMBER_ON_OFFLINE sOnOffLine;
    m_pGaeaClient->GetClubMemberOnOffline( sOnOffLine );
    std::string strText = sc::string::format( ID2GAMEINTEXT("WB_CLUB_WINDOW_BASIC_INFO_ONOFFLINE_TEXT"),
                                                sOnOffLine.m_Online, sOnOffLine.m_Total );
    m_pOnOfflineText->SetText( strText.c_str() );
}

void CWbClubBasicInfoTab::AddClubMember( club::GLCLUBMEMBER_CLIENT& sClubMember )
{
    GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();

    const bool bOnline = sClubMember.m_Online;
    const int nChannel = sClubMember.m_Channel;
    const DWORD dwID = sClubMember.m_ChaDbNum;
    bool bCDM = false;

    std::string strText = "";
    D3DCOLOR TextColor = NS_UITEXTCOLOR::DARKGRAY;

    if ( bOnline )
    {
        bCDM = pMyClub->IsMemberFlgCDM(dwID);

        if ( bCDM )            
            TextColor = NS_UITEXTCOLOR::PALEGREEN;
        else
            TextColor = NS_UITEXTCOLOR::WHITE;
    }
    else
        TextColor = NS_UITEXTCOLOR::DARKGRAY;

    strText = GetClassString( sClubMember.GetChaClass() );
    m_pClassField->AddList( strText, dwID, TextColor );

    strText = sc::string::format( "%1%", sClubMember.m_ChaLevel );
    m_pLevelField->AddList( strText, dwID, TextColor );

    strText = sc::string::format( "%1%", sClubMember.GetName() );
    m_pNameField->AddList( strText, dwID, TextColor );

    strText = sc::string::format( "%1%", nChannel );
    m_pChannelField->AddList( strText, dwID, TextColor );

    if (bOnline)
    {
        int nPosX = 0;
        int nPosY = 0;

        m_pGaeaClient->ConvertWorldToMap(sClubMember.m_CurrentMap, sClubMember.m_CurrentPos, nPosX, nPosY);
        strText = sc::string::format( "%1% %2%/%3%", sClubMember.GetMapName(), nPosX, nPosY );
    }
    else
        strText = "-";

    m_pPositionField->AddList( strText, dwID, TextColor );

    std::string strCdCertifier = "";
    DWORD dwCdCertifier = m_pGaeaClient->GetMyClub()->GetCdCertifier();
    if ( dwID == dwCdCertifier )
        strCdCertifier = sc::string::format( "%1%", ID2GAMEWORD_BASIC_INFO_MASTER_MARK_2 );

    if ( pMyClub->IsMaster( dwID ) )
    {
        strText = sc::string::format( "%1%%2%%3%",
            ID2GAMEWORD_BASIC_INFO_MASTER_MARK_0, strCdCertifier, ID2GAMEWORD_BASIC_INFO_MASTER_MARK_3 );
    }
    else if ( pMyClub->IsSubMaster( dwID ) )
    {
        strText = sc::string::format( "%1%%2%%3%",
            ID2GAMEWORD_BASIC_INFO_MASTER_MARK_1, strCdCertifier, ID2GAMEWORD_BASIC_INFO_MASTER_MARK_4 );
    }
    else
        strText = sc::string::format( "%1%%2%", strCdCertifier, sClubMember.GetGradeName() );

	// Text 가 없는 경우가 있다.
	if ( !strText.empty() )
		m_pGradeField->AddList( strText, dwID, TextColor );
}

std::string CWbClubBasicInfoTab::GetClassString( EMCHARCLASS emClass )
{
    if ( emClass & GLCC_FIGHTER )
        return ID2GAMEWORD_BASIC_INFO_CLASS_FIGHTER;
    if ( emClass & GLCC_ARMS )
        return ID2GAMEWORD_BASIC_INFO_CLASS_ARMS;
    if ( emClass & GLCC_ARCHER )
        return ID2GAMEWORD_BASIC_INFO_CLASS_ARCHER;
    if ( emClass & GLCC_SPIRIT )
        return ID2GAMEWORD_BASIC_INFO_CLASS_SPIRIT;
    if ( emClass & GLCC_EXTREME )
        return ID2GAMEWORD_BASIC_INFO_CLASS_EXTREME;
    if ( emClass & GLCC_SCIENTIST )
        return ID2GAMEWORD_BASIC_INFO_CLASS_SCIENTIST;
    if ( emClass & GLCC_ASSASSIN )
        return ID2GAMEWORD_BASIC_INFO_CLASS_ASSASSIN;
    if ( emClass & GLCC_TRICKER )
        return ID2GAMEWORD_BASIC_INFO_CLASS_TRICKER;
	if ( emClass & GLCC_ETC )
		return ID2GAMEWORD_BASIC_INFO_CLASS_ETC;

    return "-";
}

const DWORD CWbClubBasicInfoTab::GetClubMemberID()
{
    if ( !IsSelectedIndex() )
        return NATIVEID_NULL().dwID;

    return m_pClassField->GetTextData( m_nSelectIndex );
}

const DWORD CWbClubBasicInfoTab::GetClubAllianceID()
{
	return m_pRelation->GetClubAllianceID();
}

bool CWbClubBasicInfoTab::IsSelectedIndex()
{
    return ( 0 <= m_nSelectIndex && m_nSelectIndex < static_cast<int>(m_vecMembers.size()) );
}

const club::GLCLUBMEMBER_CLIENT& CWbClubBasicInfoTab::GetSelectMember()
{
    static const club::GLCLUBMEMBER_CLIENT sClubMember;
    if ( !IsSelectedIndex() )
        return sClubMember;

    if ( m_pOnlyOnlineCheck->IsChecked() )
    {
        int nOnlineCount = -1;
        BOOST_FOREACH( const club::GLCLUBMEMBER_CLIENT& sMember, m_vecMembers )
        {
            if ( sMember.m_Online )
            {
                nOnlineCount++;

                if( m_nSelectIndex == nOnlineCount )
                    return sMember;
            }            
        }

        return sClubMember;
    }
    
    return m_vecMembers[ m_nSelectIndex ];
}

bool CWbClubBasicInfoTab::IsCDAuthorizeToMaster()
{
    DWORD dwCdCertifier = m_pGaeaClient->GetMyClub()->GetCdCertifier();
    if ( dwCdCertifier == 0 || m_pGaeaClient->GetMyClub()->IsMaster( dwCdCertifier ) )
        return true;
    else
        return false;
}

void CWbClubBasicInfoTab::ShowGradePopup()
{
    if ( IsSelectedIndex() )
    {
        DWORD dwCharDbNum = GetSelectMember().m_ChaDbNum;
        const char* strCharName = GetSelectMember().GetName();
        DWORD dwGrade = GetSelectMember().GetGrade();
        m_pSetGradePopup->SetGradeData( dwCharDbNum, strCharName, dwGrade );

        m_pSetGradePopup->SetVisibleSingle( TRUE );
    }
}

void CWbClubBasicInfoTab::ShowNotice()
{
    DWORD dwCharDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();

    if ( m_pGaeaClient->GetMyClub()->IsMemberFlgNotice( dwCharDbNum ) )
        m_pNoticeEditBtn->SetVisibleSingle( TRUE );
    else
        m_pNoticeEditBtn->SetVisibleSingle( FALSE );

    __time64_t NoticeData = m_pGaeaClient->GetMyClub()->GetNoticeDate();
    if ( m_pGaeaClient->GetMyClub()->GetNoticeDate() != tDEFAULT_NOTICE_DATA )
    {
        std::string& strDate = m_pGaeaClient->GetClubNoticeDate();
        std::string& strNoticeChaName = m_pGaeaClient->GetClubNoticeChaName();
        std::string strNoticeDate = sc::string::format( ID2GAMEINTEXT("WB_CLUB_WINDOW_BASIC_INFO_NOTICE"),
            strDate, strNoticeChaName );

        m_pNoticeDate->ClearText();
        m_pNoticeDate->AddOneLineText( strNoticeDate.c_str(), true, NS_UITEXTCOLOR::WHITE );

        m_pNoticeDate->SetVisibleSingle( TRUE );
    }
    else
        m_pNoticeDate->SetVisibleSingle( FALSE );

    m_pNoticeBody->SetText( m_pGaeaClient->GetClubNotice().c_str(), NS_UITEXTCOLOR::WHITE );
}

void CWbClubBasicInfoTab::ShowClubLog()
{
    m_pActivityBody->ClearText();

    std::vector<club::LOG_DATA> vecClubLog;
    if ( !m_pGaeaClient->GetClubLog( club::LOG_ALL, 0, NUM_RECENT_ACTIVITY, vecClubLog ) )
        return;

    for (size_t i=0; i<vecClubLog.size(); ++i)
    {
        club::LOG_DATA& sLogData = vecClubLog[i];

        std::vector<sc::string::PARSE_RESULT> Split;
        sc::string::ParseMark(sLogData.m_LogText, '[', ']', Split);

        int nLine = 0;
        for (size_t i=0; i<Split.size(); ++i)
        {
            D3DCOLOR TextColor = NS_UITEXTCOLOR::WHITE;
            if (Split[i].m_Mark)
                TextColor = NS_UITEXTCOLOR::GREENYELLOW;

            if (i == 0)
                nLine = m_pActivityBody->AddText(Split[i].m_Data.c_str(), TextColor);
            else
                m_pActivityBody->AddString(nLine, Split[i].m_Data.c_str(), TextColor);
        }

        //int nLine = m_pActivityBody->AddText(sLogData.m_LogText.c_str(), NS_UITEXTCOLOR::WHITE);
        //m_pActivityBody->AddString(nLine, , );
    }
}

void CWbClubBasicInfoTab::LoadClubAllianceList()
{
    m_pRelation->ShowClubAllianceList();
}

void CWbClubBasicInfoTab::LoadClubBattleList()
{
    m_pRelation->ShowClubBattleList();
}

void CWbClubBasicInfoTab::KickMember( int ClickIndex /* = -1 */ )
{
    if ( !IsSelectedIndex() )
        return;

    std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_KICK_MEMBER"), GetSelectMember().GetName() );
    m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_KICK );
}

void CWbClubBasicInfoTab::DelegateMaster( int ClickIndex /* = -1 */ )
{
    if ( !IsSelectedIndex() )
        return;

    std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_AUTHORITY_MEMBER"), GetSelectMember().GetName() );
    m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_AUTHORITY );
}

void CWbClubBasicInfoTab::AuthorizeCdCertifier( int ClickIndex /* = -1 */ )
{
    if ( !IsSelectedIndex() )
        return;

    DWORD dwCdCertifier = m_pGaeaClient->GetMyClub()->GetCdCertifier();
    if ( dwCdCertifier == 0 || m_pGaeaClient->GetMyClub()->IsMaster( dwCdCertifier ) )
    {
        std::string strCombine = sc::string::format( ID2GAMEINTEXT("MODAL_CLUB_CD_AUTHORITY_MEMBER"), GetSelectMember().GetName() );
        m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_CD_AUTHORITY_TO_MEMBER );
    }
    else
    {
        std::string strCombine = sc::string::format( ID2GAMEINTEXT("MODAL_CLUB_CD_AUTHORITY_MASTER"), GetSelectMember().GetName() );
        m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_CD_AUTHORITY_TO_MASTER );
    }
}

void CWbClubBasicInfoTab::ChangeGrade( int ClickIndex /* = -1 */ )
{
    ShowGradePopup();
}

void CWbClubBasicInfoTab::RefreshGuidanceMap()
{
    if ( m_pRelation )
        m_pRelation->RefreshGuidanceMap();
}
