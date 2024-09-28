#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
// [12/11/2012 gbgim]
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"

#include "./WbClubBasicInfoRelation.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


const int CWbClubBasicInfoRelation::NON_SELECT = -1;

namespace
{
    const char* ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_0 = "기본정보";
    const char* ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_1 = "동맹정보";
    const char* ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_2 = "적대클럽";

    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_0 = "창설일: %1%";
    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_1 = "특별활동: %1%";
    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_2 = "배틀: %1%-%2%-%3%";
    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_3 = "월드배틀: %1%(%2%)";
    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_4 = "등급: %1%";
    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_5 = "데스매치: %1%-%2%-%3%";
    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_6 = "선도클럽: %1%";

    const char* ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_0 = "동맹 해지";
    const char* ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_1 = "휴전";
    const char* ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_2 = "항복";

    const char* ID2WORD_WB_CLUB_WIDNOW_RADIO_RELATIONTAB_0 = "Kill: %1% / Death: %2%";
}

CWbClubBasicInfoRelation::CWbClubBasicInfoRelation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pRadioBasicInfo(NULL)
    , m_pRadioAlliance(NULL)
    , m_pRadioEnemy(NULL)
    , m_pBasicInfo0(NULL)
    , m_pBasicInfo1(NULL)
    , m_pBasicScroll(NULL)
    , m_pRelationBody(NULL)
    , m_pRelation0(NULL)
    , m_pRelation1(NULL)
    , m_pRelationScroll(NULL)
    , m_pSelect(NULL)
    , m_pSepLine(NULL)
    , m_pAllianceBtn(NULL)
    , m_pArmisticeBtn(NULL)
    , m_pSubmissionBtn(NULL)
	, m_pClubChiefToolTip( NULL )
    , m_nCurRadioTab(NON_SELECT)
    , m_nSelectIndex(NON_SELECT)
{
    m_vecClubAllinace.clear();
    m_vecClubBattle.clear();
    m_pClubMarkVec.clear();

    ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_0 = ID2GAMEWORD("WB_CLUB_WINDOW_BASIC_RADIO", 0);
    ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_1 = ID2GAMEWORD("WB_CLUB_WINDOW_BASIC_RADIO", 1);
    ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_2 = ID2GAMEWORD("WB_CLUB_WINDOW_BASIC_RADIO", 2);

    ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_0 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_BASICTAB", 0);
    ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_1 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_BASICTAB", 1);
    ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_2 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_BASICTAB", 2);
    ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_3 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_BASICTAB", 3);
    ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_4 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_BASICTAB", 4);
    ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_5 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_BASICTAB", 5);
    ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_6 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_BASICTAB", 6);

    ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_0 = ID2GAMEWORD("WB_CLUB_WINDOW_RADIO_RELATION_BTN", 0);
    ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_1 = ID2GAMEWORD("WB_CLUB_WINDOW_RADIO_RELATION_BTN", 1);
    ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_2 = ID2GAMEWORD("WB_CLUB_WINDOW_RADIO_RELATION_BTN", 2);

    ID2WORD_WB_CLUB_WIDNOW_RADIO_RELATIONTAB_0 = ID2GAMEWORD("WB_CLUB_WIDNOW_RADIO_RELATIONTAB", 0);
}

CWbClubBasicInfoRelation::~CWbClubBasicInfoRelation()
{

}

void CWbClubBasicInfoRelation::CreateSubControl()
{
    {
        CBasicLineBoxSmart* pLinBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );;
        pLinBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_TITLE_BACK", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLinBoxSmart->CreateSubControl( "SELCET_CLUB_LIST_FIELD_LINE" );
        RegisterControl( pLinBoxSmart );
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        DEFAULT_RADIO_BUTTON* pBoardRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pBoardRadio->CreateSub(this,"WB_CLUB_WINDOW_BASIC_INFO_RELATION_BASIC",UI_FLAG_DEFAULT, RADIO_BASIC_INFO);
        pBoardRadio->CreateSubControl( ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_0, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y , "WB_CLUB_WINDOW_BASIC_INFO_RELATION_TEXT", NS_UITEXTUTIL::WHITE, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_CHECK", "CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO", "CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO", pFont9Shadow);
        RegisterTapButton(pBoardRadio);
        RegisterControl(pBoardRadio);
        m_pRadioBasicInfo = pBoardRadio;

        pBoardRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pBoardRadio->CreateSub(this,"WB_CLUB_WINDOW_BASIC_INFO_RELATION_ALLIANCE",UI_FLAG_DEFAULT, RADIO_ALLIANCE);
        pBoardRadio->CreateSubControl( ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_1, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y , "WB_CLUB_WINDOW_BASIC_INFO_RELATION_TEXT", NS_UITEXTUTIL::WHITE, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_CHECK", "CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO", "CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO", pFont9Shadow);
        RegisterTapButton(pBoardRadio);
        RegisterControl(pBoardRadio);
        m_pRadioAlliance = pBoardRadio;

        pBoardRadio = new DEFAULT_RADIO_BUTTON(m_pEngineDevice);
        pBoardRadio->CreateSub(this,"WB_CLUB_WINDOW_BASIC_INFO_RELATION_ENEMY",UI_FLAG_DEFAULT, RADIO_ENEMY);
        pBoardRadio->CreateSubControl( ID2WORD_WB_CLUB_WINDOW_BASIC_RADIO_2, TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y , "WB_CLUB_WINDOW_BASIC_INFO_RELATION_TEXT", NS_UITEXTUTIL::WHITE, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_CHECK", "CHARACTER_WINDOW_RADIO_ON_TEXTUREINFO", "CHARACTER_WINDOW_RADIO_OFF_TEXTUREINFO", pFont9Shadow);
        RegisterTapButton(pBoardRadio);
        RegisterControl(pBoardRadio);
        m_pRadioEnemy = pBoardRadio;
    }

    {
        CUIControl* pMouseOverLine = new CUIControl(m_pEngineDevice);
        pMouseOverLine->CreateSub( this, "SELCET_COUNTRY_WINDOW_NODE_SELECT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pMouseOverLine->SetVisibleSingle( FALSE );
        pMouseOverLine->SetUseRender( TRUE );
        RegisterControl( pMouseOverLine );
        m_pSelect = pMouseOverLine;
    }

    {
        CUIControl* pControl = new CUIControl( m_pEngineDevice );
        pControl->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BODY_BOX" );
        pControl->SetVisibleSingle( FALSE );
        RegisterControl( pControl );
        m_pRelationBody = pControl;
    }

    {
        for ( int i = 0; i < static_cast<int>(GLCONST_CHAR::dwCLUB_ALLIANCE_NUM); i++ )
        {
            CUIControl* pControl = new CUIControl( m_pEngineDevice );
            pControl->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_MARK" );
            RegisterControl( pControl );
            pControl->SetVisibleSingle( FALSE );
            pControl->SetUseRender( TRUE );
            m_pClubMarkVec.push_back( pControl );
        }

        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_BASIC_BODY_0", UI_FLAG_DEFAULT, BASIC_INFO_0 );
        pTextBox->SetPartInterval( 8.0f );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        pTextBox->SetVisibleSingle( FALSE );
        m_pBasicInfo0 = pTextBox;

        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_BASIC_BODY_1", UI_FLAG_DEFAULT, BASIC_INFO_1 );
        pTextBox->SetPartInterval( 8.0f );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        pTextBox->SetVisibleSingle( FALSE );
        m_pBasicInfo1 = pTextBox;

        //	스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BODY_SCROLL" );
        pScrollBar->GetThumbFrame()->SetState( 1, 0 );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pBasicInfo1 );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 2 );
        RegisterControl( pScrollBar );
        pScrollBar->SetVisibleSingle( FALSE );
        m_pBasicScroll = pScrollBar;
    }

    {
        CD3DFontPar* pFont9Shadow = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

        CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BODY_0", UI_FLAG_DEFAULT, RELATION_0 );
        pTextBox->SetPartInterval( 3.0f );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        pTextBox->SetVisibleSingle( FALSE );
        m_pRelation0 = pTextBox;

        pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
        pTextBox->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BODY_1", UI_FLAG_DEFAULT, RELATION_1 );
        pTextBox->SetPartInterval( 3.0f );
        pTextBox->SetFont( pFont9Shadow );
        pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );
        RegisterControl( pTextBox );
        pTextBox->SetVisibleSingle( FALSE );
        m_pRelation1 = pTextBox;

        //	스크롤바
        CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pScrollBar->CreateBaseScrollBar( "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BODY_SCROLL" );
        pScrollBar->GetThumbFrame()->SetState( 1, 0 );
        pScrollBar->GetThumbFrame()->SetScrollParent( m_pRelationBody );
        pScrollBar->GetThumbFrame()->SetScrollMoveLine( 2 );
        RegisterControl( pScrollBar );
        pScrollBar->SetVisibleSingle( FALSE );
        m_pRelationScroll = pScrollBar;
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

        arg.text = ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_0;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BTN_1", UI_FLAG_DEFAULT, ALLIANCE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pAllianceBtn = pRnButton;

        arg.text = ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_1;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BTN_0", UI_FLAG_DEFAULT, ARMISTICE_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pArmisticeBtn = pRnButton;

        arg.text = ID2WORD_WB_CLUB_WINDOW_RADIO_RELATION_BTN_2;
        pRnButton = new RnButton(m_pEngineDevice);
        pRnButton->CreateSub(this, "WB_CLUB_WINDOW_BASIC_INFO_RELATION_BTN_1", UI_FLAG_DEFAULT, SUBMISSION_BTN );
        pRnButton->CreateSubControl(arg);
        RegisterControl(pRnButton);
        pRnButton->SetVisibleSingle( FALSE );
        m_pSubmissionBtn = pRnButton;
    }

    {
        CBasicLineBoxSmart* pLineBoxSmart = new CBasicLineBoxSmart( m_pEngineDevice );
        pLineBoxSmart->CreateSub( this, "WB_CLUB_WINDOW_BASIC_INFO_BASIC_BODY_SEPLINE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBoxSmart->CreateSubControl( "WB_CLUB_WINDOW_TAB_BASIC_TAB_SEPLINE_BACK_LINE" );
        RegisterControl( pLineBoxSmart );
        m_pSepLine = pLineBoxSmart;
    }

	// [12/11/2012 gbgim]
	{
		m_pClubChiefToolTip = new CBasicVarTextBox( m_pEngineDevice );
		m_pClubChiefToolTip->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pClubChiefToolTip->CreateSubControl();
		m_pClubChiefToolTip->SetVisibleSingle( FALSE );
		RegisterControl( m_pClubChiefToolTip );
		m_pClubChiefToolTip->AddTextNoSplit( ID2GAMEINTEXT("CLUB_ALLIANCE_CHIEF_TOOLTIP_TEXT"), NS_UITEXTCOLOR::WHITE );
		UIRECT rcClubChiefToolTip = m_pClubChiefToolTip->CalcSize();
		m_pClubChiefToolTip->SetControlSize( (int)rcClubChiefToolTip.sizeX, (int)rcClubChiefToolTip.sizeY );
	}

    EventSelectedTap( RADIO_BASIC_INFO );
}

void CWbClubBasicInfoRelation::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	// [12/12/2012 gbgim]
	ResetToolTip();

    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pBasicScroll->GetThumbFrame();

        const int nTotalLine = m_pBasicInfo1->GetTotalLine();
        const int nLinePerOneView = m_pBasicInfo1->GetVisibleLine();

        pThumbFrame->SetState( nTotalLine, nLinePerOneView );
        if ( nLinePerOneView < nTotalLine )
        {
            const int nMovableLine = nTotalLine - nLinePerOneView;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = static_cast<int>(floor(fPercent * nMovableLine));

            if ( m_pBasicInfo1->GetCurLine() != nPos )
            {
                m_pBasicInfo1->SetCurLine( nPos );
            }
        }
    }

    {
        CBasicScrollThumbFrame* const pThumbFrame = m_pRelationScroll->GetThumbFrame();

        const int nTotalLine = m_pRelation0->GetTotalLine();
        const int nLinePerOneView = m_pRelation0->GetVisibleLine();

        pThumbFrame->SetState( nTotalLine, nLinePerOneView );
        if ( nLinePerOneView < nTotalLine )
        {
            const int nMovableLine = nTotalLine - nLinePerOneView;
            float fPercent = pThumbFrame->GetPercent();
            int nPos = static_cast<int>(floor(fPercent * nMovableLine));

            if ( m_pRelation0->GetCurLine() != nPos )
            {
                m_pRelation0->SetCurLine( nPos );

                ShowClubMarks();

                UnSelect();
            }
        }
    }
}

void CWbClubBasicInfoRelation::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );

    switch ( ControlID )
    {
    case RELATION_0:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                if ( m_nCurRadioTab == RADIO_ENEMY )
                {
                    int SelectPos = m_pRelation0->GetSelectPos();
                    int VecClubBattleSize = static_cast<int>(m_vecClubBattle.size());

                    if ( SelectPos == NON_SELECT || VecClubBattleSize <= SelectPos )
                        return;

                    GLCLUBBATTLE& ClubBattle = m_vecClubBattle[ SelectPos ];

                    std::string StartTimeInfo = sc::time::DateTimeFormat( ClubBattle.m_tStartTime, true, false );
                    std::string EndTimeInfo = sc::time::DateTimeFormat( ClubBattle.m_tEndTime, true, false );
                    __time64_t EndAbleTime = ClubBattle.m_tStartTime;
                    EndAbleTime += (GLCONST_CHAR::dwCLUB_BATTLE_DIS_TIME*60);
                    std::string EndAbleTimeInfo = sc::time::DateTimeFormat( EndAbleTime, true, false );

                    m_pInterface->BEGIN_COMMON_LINEINFO_MULTI();

                    std::string ClubBattleInfo = stringformat(
                        ID2GAMEINTEXT("WB_CLUB_WINDOW_CLUB_BATTLE_TOOLTIP_0"), StartTimeInfo);
                    m_pInterface->ADD_COMMON_LINEINFO_MULTI( ClubBattleInfo.c_str(), NS_UITEXTCOLOR::WHITE );

                    ClubBattleInfo = stringformat(
                        ID2GAMEINTEXT("WB_CLUB_WINDOW_CLUB_BATTLE_TOOLTIP_1"), EndTimeInfo);
                    m_pInterface->ADD_COMMON_LINEINFO_MULTI( ClubBattleInfo.c_str(), NS_UITEXTCOLOR::WHITE );

                    ClubBattleInfo = stringformat(
                        ID2GAMEINTEXT("WB_CLUB_WINDOW_CLUB_BATTLE_TOOLTIP_2"), EndAbleTimeInfo);
                    m_pInterface->ADD_COMMON_LINEINFO_MULTI( ClubBattleInfo.c_str(), NS_UITEXTCOLOR::WHITE );

                    m_pInterface->END_COMMON_LINEINFO_MULTI();
                }
				// [12/11/2012 gbgim]
				else if( m_nCurRadioTab == RADIO_ALLIANCE )
				{
					int nSelectIndex = m_pRelation0->GetSelectPos();
					int nVecClubAllianceSize = static_cast<int>(m_vecClubAllinace.size());

					if ( nSelectIndex == NON_SELECT || nVecClubAllianceSize <= nSelectIndex )
						return;

					GLCLUBALLIANCE& ClubAlliance = m_vecClubAllinace[nSelectIndex];
					if( ClubAlliance.m_dwID == m_pGaeaClient->GetChiefID() )
						m_pClubChiefToolTip->SetVisibleSingle( TRUE );
				}
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                UIRECT rcPart;
                if ( m_pRelation0->GetPartInfo( m_pRelation0->GetSelectPos(), rcPart) )
                {
                    m_nSelectIndex = m_pRelation0->GetSelectPos();
                    SelectList( rcPart );
                    ShowRelationBtns();
                }
                else
                    UnSelect();
            }
        }
        break;

    case RELATION_1:
        {
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {
                if ( m_nCurRadioTab == RADIO_ENEMY )
                {
                    int SelectPos = m_pRelation1->GetSelectPos();
                    int VecClubBattleSize = static_cast<int>(m_vecClubBattle.size());

                    if ( SelectPos == NON_SELECT || VecClubBattleSize <= SelectPos )
                        return;

                    GLCLUBBATTLE& ClubBattle = m_vecClubBattle[ SelectPos ];

                    std::string StartTimeInfo = sc::time::DateTimeFormat( ClubBattle.m_tStartTime, true, false );
                    std::string EndTimeInfo = sc::time::DateTimeFormat( ClubBattle.m_tEndTime, true, false );
                    __time64_t EndAbleTime = ClubBattle.m_tStartTime;
                    EndAbleTime += (GLCONST_CHAR::dwCLUB_BATTLE_DIS_TIME*60);
                    std::string EndAbleTimeInfo = sc::time::DateTimeFormat( EndAbleTime, true, false );

                    m_pInterface->BEGIN_COMMON_LINEINFO_MULTI();

                    std::string ClubBattleInfo = stringformat(
                        ID2GAMEINTEXT("WB_CLUB_WINDOW_CLUB_BATTLE_TOOLTIP_0"), StartTimeInfo);
                    m_pInterface->ADD_COMMON_LINEINFO_MULTI( ClubBattleInfo.c_str(), NS_UITEXTCOLOR::WHITE );

                    ClubBattleInfo = stringformat(
                        ID2GAMEINTEXT("WB_CLUB_WINDOW_CLUB_BATTLE_TOOLTIP_1"), EndTimeInfo);
                    m_pInterface->ADD_COMMON_LINEINFO_MULTI( ClubBattleInfo.c_str(), NS_UITEXTCOLOR::WHITE );

                    ClubBattleInfo = stringformat(
                        ID2GAMEINTEXT("WB_CLUB_WINDOW_CLUB_BATTLE_TOOLTIP_2"), EndAbleTimeInfo);
                    m_pInterface->ADD_COMMON_LINEINFO_MULTI( ClubBattleInfo.c_str(), NS_UITEXTCOLOR::WHITE );

                    m_pInterface->END_COMMON_LINEINFO_MULTI();
                }
				// [12/11/2012 gbgim]
				else if( m_nCurRadioTab == RADIO_ALLIANCE )
				{
					int nSelectIndex = m_pRelation1->GetSelectPos();
					int nVecClubAllianceSize = static_cast<int>(m_vecClubAllinace.size());

					if ( nSelectIndex == NON_SELECT || nVecClubAllianceSize <= nSelectIndex )
						return;

					GLCLUBALLIANCE& ClubAlliance = m_vecClubAllinace[nSelectIndex];
					if( ClubAlliance.m_dwID == m_pGaeaClient->GetChiefID() )
						m_pClubChiefToolTip->SetVisibleSingle( TRUE );
				}
            }

            if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
            {
                UIRECT rcPart;
                if ( m_pRelation1->GetPartInfo( m_pRelation1->GetSelectPos(), rcPart) )
                {
                    m_nSelectIndex = m_pRelation1->GetSelectPos();
                    SelectList( rcPart );
                    ShowRelationBtns();
                }
                else
                    UnSelect();
            }
        }
        break;

	// 동맹 해지 버튼
    case ALLIANCE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                if ( IsSelected( m_vecClubAllinace.size() ) )
                {
					if( m_pGaeaClient->IsChief() )
					{
						m_pInterface->DoModal( ID2GAMEINTEXT("CLUB_ALLIANCE_KICK"),
							UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_UNION_KICK );
					}
					else
					{
						m_pInterface->DoModal( ID2GAMEINTEXT("CLUB_ALLIANCE_WITHDRAW"),
							UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_UNION_WITHDRAW );
					}
				}
            }
        }
        break;

    case ARMISTICE_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                if ( IsSelected( m_vecClubBattle.size() ) )
                {
					DWORD dwClubDbNum = m_vecClubBattle[m_nSelectIndex].m_dwCLUBID;

					std::vector<GLCLUBALLIANCE> vecAlliance;
					m_pGaeaClient->GetClubAlliance( vecAlliance );

					if ( vecAlliance.size() == 0)
						m_pGaeaClient->ReqClubBattleArmistice( dwClubDbNum );
					else
						m_pGaeaClient->ReqAllianceBattleArmistice( dwClubDbNum );
                }

                UnSelect();
            }
        }
        break;

    case SUBMISSION_BTN:
        {
            if ( UIMSG_RNBUTTON_CLICK & dwMsg )
            {
                if ( IsSelected( m_vecClubBattle.size() ) )
                {
                    DWORD dwClubDbNum = m_vecClubBattle[m_nSelectIndex].m_dwCLUBID;

                    std::vector<GLCLUBALLIANCE> vecAlliance;
                    m_pGaeaClient->GetClubAlliance( vecAlliance );

                    if ( vecAlliance.size() == 0)
                        m_pGaeaClient->ReqClubBattleSubmission( dwClubDbNum );
                    else
                        m_pGaeaClient->ReqAllianceBattleSubmission( dwClubDbNum );
                }

                UnSelect();
            }
        }
        break;
    };

    EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
    TapSelectorUIMessage( ControlID, dwMsg, emResult );
}

void CWbClubBasicInfoRelation::EventSelectedTap( UIGUID ControlID )
{
    if ( m_nCurRadioTab != ControlID )
    {
        AllUnshow();
        UnSelect();
    }

    m_nCurRadioTab = ControlID;

    switch ( ControlID )
    {
    case RADIO_BASIC_INFO:
        {
            m_pGaeaClient->ReqGuidanceMapName();
            ShowBasicInfo();
        }
        break;

    case RADIO_ALLIANCE:
        {
            ShowClubAllianceList();
        }
        break;

    case RADIO_ENEMY:
        {
            ShowClubBattleList();
        }
        break;
    };
}

HRESULT CWbClubBasicInfoRelation::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr( S_OK );
	V_RETURN( CUIGroup::RestoreDeviceObjects( pd3dDevice ) );

	UIRECT rcPart1;
	if ( m_pRelation0->GetPartInfo( m_nSelectIndex, rcPart1 ) )
		SelectList( rcPart1 );
	else
		UnSelect();

	UIRECT rcPart2;
	if ( m_pRelation1->GetPartInfo( m_nSelectIndex, rcPart2 ) )
		SelectList( rcPart2 );
	else
		UnSelect();

	return hr;
}

void CWbClubBasicInfoRelation::AllUnshow()
{
    if ( m_pBasicInfo0 )
        m_pBasicInfo0->SetVisibleSingle( FALSE );
    if ( m_pBasicInfo1 )
        m_pBasicInfo1->SetVisibleSingle( FALSE );
    if ( m_pSepLine )
        m_pSepLine->SetVisibleSingle( FALSE );
    if ( m_pBasicScroll )
        m_pBasicScroll->SetVisibleSingle( FALSE );

    if ( m_pRelation0 )
        m_pRelation0->SetVisibleSingle( FALSE );
    if ( m_pRelation1 )
        m_pRelation1->SetVisibleSingle( FALSE );
    if ( m_pRelationScroll )
        m_pRelationScroll->SetVisibleSingle( FALSE );
    
    if ( m_pAllianceBtn )
        m_pAllianceBtn->SetVisibleSingle( FALSE );
    if ( m_pArmisticeBtn )
        m_pArmisticeBtn->SetVisibleSingle( FALSE );
    if ( m_pSubmissionBtn )
        m_pSubmissionBtn->SetVisibleSingle( FALSE );

    if ( m_pSelect )
        m_pSelect->SetVisibleSingle( FALSE );

    for ( size_t i = 0; i < m_pClubMarkVec.size(); i++ )
        m_pClubMarkVec[i]->SetVisibleSingle( FALSE );

	//  [12/11/2012 gbgim]
	if( m_pClubChiefToolTip )
		m_pClubChiefToolTip->SetVisibleSingle( FALSE );
}

void CWbClubBasicInfoRelation::ResetToolTip()
{
	if( m_pClubChiefToolTip )
		m_pClubChiefToolTip->SetVisibleSingle( FALSE );
}

void CWbClubBasicInfoRelation::ShowBasicInfo()
{
    if ( !m_pBasicInfo0 || !m_pBasicInfo1 )
        return;

    if ( m_nCurRadioTab != RADIO_BASIC_INFO )
        return;

    m_pBasicInfo0->ClearText();
    m_pBasicInfo1->ClearText();

    std::string strCreateDate = m_pGaeaClient->GetClubCreateDate();
    DWORD dwActivityPoint = m_pGaeaClient->GetClubActivityPoint();
    DWORD dwClubBattleWin, dwClubBattleDraw, dwClubBattleLost;
    m_pGaeaClient->GetClubBattleRecord( dwClubBattleWin, dwClubBattleDraw, dwClubBattleLost );
    DWORD dwWbPoint, dwWbRank;
    m_pGaeaClient->GetClubWorldBattleRecord( dwWbPoint, dwWbRank );
    std::string strClubRank = m_pGaeaClient->GetClubRank();
    DWORD CDM_Ranking, CDM_Kill, CDM_Death;
    m_pGaeaClient->GetClubDeathMatchRecord( CDM_Ranking, CDM_Kill, CDM_Death );
    std::vector<std::string> vecGuidance;
    m_pGaeaClient->GetClubGuidanceMap( vecGuidance );

    std::string strText = sc::string::format( ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_0, strCreateDate );
    m_pBasicInfo0->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    strText = sc::string::format( ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_1, dwActivityPoint );
    m_pBasicInfo0->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    strText = sc::string::format( ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_2,
        dwClubBattleWin, dwClubBattleDraw, dwClubBattleLost );
    m_pBasicInfo0->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    // Note : 기획팀 요청으로 월드클럽 배틀 정보는 아직 출력하지 않는다.
    /*strText = sc::string::format( ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_3,
        dwWbPoint, dwWbRank );
    m_pBasicInfo0->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );*/

    strText = sc::string::format( ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_4, strClubRank );
    m_pBasicInfo0->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    strText = sc::string::format( ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_5,
        CDM_Ranking, CDM_Kill, CDM_Death );
    m_pBasicInfo1->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );

    if ( !vecGuidance.empty() )
    {
        strText = sc::string::format( "%1%", ID2WORD_WB_CLUB_WIDNOW_RADIO_BASICTAB_6 );
        m_pBasicInfo1->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );

        for ( size_t i = 0; i < vecGuidance.size(); i++ )
        {
            strText = sc::string::format( "[%1%]", vecGuidance[i] );
            m_pBasicInfo1->AddText( strText.c_str(), NS_UITEXTCOLOR::WHITE );
        }
    }

    m_pBasicInfo0->SetVisibleSingle( TRUE );
    m_pBasicInfo1->SetVisibleSingle( TRUE );
    m_pSepLine->SetVisibleSingle( TRUE );
    m_pBasicScroll->SetVisibleSingle( TRUE );
}

void CWbClubBasicInfoRelation::ShowClubAllianceList()
{
    if ( !m_pRelation0 || !m_pRelation1 || !m_pRelationScroll || !m_pAllianceBtn )
        return;

    if ( m_nCurRadioTab != RADIO_ALLIANCE )
        return;

    m_pRelation0->ClearText();
    m_pRelation1->ClearText();

    m_vecClubAllinace.clear();
    m_pGaeaClient->GetClubAlliance( m_vecClubAllinace );

    for ( size_t i = 0; i< m_vecClubAllinace.size(); i++ )
    {
        GLCLUBALLIANCE& ClubAlliance = m_vecClubAllinace[i];
        m_pRelation0->AddTextNoSplit( ClubAlliance.GetClubName(), NS_UITEXTCOLOR::WHITE );
        m_pRelation1->AddTextNoSplit( ClubAlliance.GetClubMasterName(), NS_UITEXTCOLOR::WHITE );
    }

    m_pRelation0->SetVisibleSingle( TRUE );
    m_pRelation1->SetVisibleSingle( TRUE );
    m_pRelationScroll->SetVisibleSingle( TRUE );

    ShowClubMarks();
}

void CWbClubBasicInfoRelation::ShowClubBattleList()
{
    if ( !m_pRelation0 || !m_pRelation1 || !m_pRelationScroll ||
        !m_pArmisticeBtn || !m_pSubmissionBtn )
        return;

    if ( m_nCurRadioTab != RADIO_ENEMY )
        return;

    m_pRelation0->ClearText();
    m_pRelation1->ClearText();

    m_vecClubBattle.clear();
    m_pGaeaClient->GetClubBattleData( m_vecClubBattle );

    for ( size_t i = 0; i < m_vecClubBattle.size(); i++ )
    {
        GLCLUBBATTLE& ClubBattle = m_vecClubBattle[i];
        m_pRelation0->AddTextNoSplit( ClubBattle.m_szClubName, NS_UITEXTCOLOR::WHITE );

        std::string strText = sc::string::format( ID2WORD_WB_CLUB_WIDNOW_RADIO_RELATIONTAB_0,
            ClubBattle.m_wKillPoint, ClubBattle.m_wDeathPoint );
        m_pRelation1->AddTextNoSplit( strText.c_str(), NS_UITEXTCOLOR::WHITE );
    }

    m_pRelation0->SetVisibleSingle( TRUE );
    m_pRelation1->SetVisibleSingle( TRUE );
    m_pRelationScroll->SetVisibleSingle( TRUE );

    ShowClubMarks();
}

void CWbClubBasicInfoRelation::ShowRelationBtns()
{
    switch ( m_nCurRadioTab )
    {
    case RADIO_ALLIANCE:
        {
            m_pAllianceBtn->SetVisibleSingle( TRUE );
        }
        break;

    case RADIO_ENEMY:
        {
            m_pArmisticeBtn->SetVisibleSingle( TRUE );
            m_pSubmissionBtn->SetVisibleSingle( TRUE );
        }
        break;
    };
}

void CWbClubBasicInfoRelation::ShowClubMarks()
{
    for ( size_t i = 0; i < m_pClubMarkVec.size(); i++ )
        m_pClubMarkVec[i]->SetVisibleSingle( FALSE );

    DWORD dwClubDbNum, dwClubMarkVer;

    int nTotalLine = m_pRelation0->GetTotalLine();
    int MarkVecSize = static_cast<int>( m_pClubMarkVec.size() );
    for ( int nIndex = 0; nIndex < MarkVecSize && nIndex < nTotalLine; nIndex++ )
    {
        switch ( m_nCurRadioTab )
        {
        case RADIO_ALLIANCE:
            {
                if (  nIndex < static_cast<int>(m_vecClubAllinace.size()) )
                {
                    dwClubDbNum = m_vecClubAllinace[ nIndex ].GetClubDbNum();
                    dwClubMarkVer = m_vecClubAllinace[ nIndex ].GetMarkVersion();
                }
            }
            break;

        case RADIO_ENEMY:
            {
                if (  nIndex < static_cast<int>(m_vecClubBattle.size()) )
                {
                    dwClubDbNum = m_vecClubBattle[ nIndex ].m_dwCLUBID;
                    dwClubMarkVer = 0;
                }
            }
            break;
        };

        if ( nIndex < static_cast<int>(m_pClubMarkVec.size()) )
        {
            // club mark texture.
            const clubmark::DXDATA& ClubMkark = m_pGaeaClient->GetClubMark( dwClubDbNum, dwClubMarkVer );

            CUIControl* pClubMark = m_pClubMarkVec[ nIndex ];
            pClubMark->SetTexturePos( 0, ClubMkark.vTex_1_LU );
            pClubMark->SetTexturePos( 1, ClubMkark.vTex_2_RU );
            pClubMark->SetTexturePos( 3, ClubMkark.vTex_3_LD );
            pClubMark->SetTexturePos( 2, ClubMkark.vTex_4_RD );
            pClubMark->SetTexture( ClubMkark.pddsTexture );

            // club mark position.
            UIRECT rcPart;
            m_pRelation0->GetPartInfo( nIndex, rcPart );
            const UIRECT& rcMarkGP = pClubMark->GetGlobalPos();
            D3DXVECTOR2 vPos;
            vPos.x = rcMarkGP.left;
            vPos.y = rcPart.top;
            pClubMark->SetGlobalPos( vPos );

            pClubMark->SetVisibleSingle( TRUE );
        }
    }
}

bool CWbClubBasicInfoRelation::IsSelected( size_t MaxSize )
{
    if ( m_nSelectIndex < 0 || m_nSelectIndex >= static_cast<int>(MaxSize) )
        return false;
    else
        return true;
}

void CWbClubBasicInfoRelation::SelectList( UIRECT& rcPart )
{
    rcPart.left = m_pRelationBody->GetGlobalPos().left;
    rcPart.sizeX = m_pRelationBody->GetLocalPos().sizeX;
    rcPart.right = rcPart.left + rcPart.sizeX;

    m_pSelect->SetGlobalPos( rcPart );
    m_pSelect->SetVisibleSingle( TRUE );
    m_pSelect->SetDiffuse( D3DCOLOR_ARGB(0xFF,0x69,0x69,0x69) );
}

void CWbClubBasicInfoRelation::UnSelect()
{
    m_nSelectIndex = NON_SELECT;

    if ( m_pSelect )
        m_pSelect->SetVisibleSingle( FALSE );

    if ( m_pAllianceBtn )
        m_pAllianceBtn->SetVisibleSingle( FALSE );
    if ( m_pArmisticeBtn )
        m_pArmisticeBtn->SetVisibleSingle( FALSE );
    if ( m_pSubmissionBtn )
        m_pSubmissionBtn->SetVisibleSingle( FALSE );
}

int CWbClubBasicInfoRelation::GetCurTab()
{
    return m_nCurRadioTab;
}

bool CWbClubBasicInfoRelation::IsSelectedIndex()
{
	return ( 0 <= m_nSelectIndex && m_nSelectIndex < static_cast<int>(m_vecClubAllinace.size()) );
}

const DWORD CWbClubBasicInfoRelation::GetClubAllianceID()
{
	if ( !IsSelectedIndex() )
		return NON_SELECT;

	// 선택한 리스트 저장 후 UnSelect
	DWORD selectIndex = m_nSelectIndex;
	UnSelect();

	return m_vecClubAllinace[selectIndex].GetClubDbNum();
}

void CWbClubBasicInfoRelation::RefreshGuidanceMap()
{
    ShowBasicInfo();
}
