#include "StdAfx.h"
#include "SkillWindow.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include <set>
#include "SkillPage.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "SkillSlot.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSkillWindow::CSkillWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pRestPoint(NULL)
    , m_dwSkillPointBACK(0)
{
    memset ( m_pPage, 0, sizeof ( m_pPage ) );
    memset ( m_pPageButton, 0, sizeof ( m_pPageButton ) );
}

CSkillWindow::~CSkillWindow ()
{
}

void CSkillWindow::SetVisiblePageForTutorial()
{
    int nPage = SKILL_WINDOW_PAGE3;
    if ( nPage < SKILL_WINDOW_PAGE0 || SKILL_WINDOW_PAGE3 < nPage )
    {
        GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
        return ;
    }

    //	리셋
    for ( int i = 0; i < nMAXPAGE; i++ )
    {
        m_pPage[i]->SetVisibleSingle ( FALSE );
        m_pPageButton[i]->SetFlip ( FALSE );
    }

    int nIndex = nPage - SKILL_WINDOW_PAGE0;
    m_pPage[nIndex]->SetVisibleSingle ( TRUE );
    m_pPageButton[nIndex]->SetFlip ( TRUE );
}

void CSkillWindow::SetVisiblePage ( int nPage )
{
    if ( nPage < SKILL_WINDOW_PAGE0 || SKILL_WINDOW_PAGE3 < nPage )
    {
        GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
        return ;
    }

    //	리셋
    for ( int i = 0; i < nMAXPAGE; i++ )
    {
        m_pPage[i]->SetVisibleSingle ( FALSE );
        m_pPageButton[i]->SetFlip ( FALSE );
    }

    int nIndex = nPage - SKILL_WINDOW_PAGE0;
    m_pPage[nIndex]->SetVisibleSingle ( TRUE );
    m_pPageButton[nIndex]->SetFlip ( TRUE );
}

void CSkillWindow::CreateSubControl ()
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    const DxCharJoinData* pJoinData = pGlobalStage->GetGameStage()->GetCharJoinData();
    EMCHARCLASS emCharClass = pJoinData->m_CharData2.m_emClass;

    int nCLASSTYPE = CharClassToClassIndex(emCharClass);

    int nSkillCountAll[EMSKILL_PC_CLASSNUM] = { 0, 0, 0, 0, 0, 0 };

    SNATIVEID sNativeIDArrayAll[EMSKILL_PC_CLASSNUM][GLSkillMan::MAX_CLASSSKILL];
    {	//	스킬 정렬 ( 등급 순서 )		
        for ( int i = 0; i < EMSKILL_PC_CLASSNUM; i ++ )
        {
            for ( int j = 0; j < GLSkillMan::MAX_CLASSSKILL; j++ )
            {
                sNativeIDArrayAll[i][j] = NATIVEID_NULL();
            }
        }

        SortSkill ( emCharClass, sNativeIDArrayAll, nSkillCountAll );
    }

    {	//	탭 버튼
        const CString strClassTextKeyword[GLCL_NUM_CLASS] = 
        {
            "SKILL_TAB_TITLE_FIGHTER",
            "SKILL_TAB_TITLE_ARMS",
            "SKILL_TAB_TITLE_ARCHER",
            "SKILL_TAB_TITLE_SPIRIT",
            "SKILL_TAB_TITLE_EXTREME_M",
            "SKILL_TAB_TITLE_SCIENTIST",
            "SKILL_TAB_TITLE_ASSASSIN",
			"SKILL_TAB_TITLE_TRICKER",
            "SKILL_TAB_TITLE_ETC",
        };

        CString strClassText = strClassTextKeyword[nCLASSTYPE];

        CString	strPageKeyword[nMAXPAGE] = 
        {
            "SKILL_WINDOW_PAGE_BUTTON0",
            "SKILL_WINDOW_PAGE_BUTTON1",
            "SKILL_WINDOW_PAGE_BUTTON2",
            "SKILL_WINDOW_PAGE_BUTTON3",
        };		

        for ( int i = 0; i < nMAXPAGE; i++ )
        {			
            m_pPage[i] = CreateSkillPage ( SKILL_WINDOW_PAGE0 + i, sNativeIDArrayAll[i], nSkillCountAll[i] );
            m_pPageButton[i] = CreateTextButton ( strPageKeyword[i].GetString (), SKILL_WINDOW_PAGE_BUTTON0 + i, (char*)ID2GAMEWORD ( strClassText.GetString(), i ) );
        }	
    }

    {	//	스킬포인트 잔량 표시
        D3DCOLOR dwColor = NS_UITEXTCOLOR::DEFAULT;
        CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
        int	nTextAlign = TEXT_ALIGN_LEFT;

        CBasicTextBox* pTextBox = CreateStaticControl ( "SKILL_WINDOW_REST_POINT_STATIC", pFont9, dwColor, nTextAlign );
        pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SKILL_WINDOW_REST_POINT_STATIC" ) );

        CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
        pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SKILL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateBaseBoxSkill ( "SKILL_WINDOW_REST_POINT_BACK" );
        RegisterControl ( pLineBox );

        m_pRestPoint = CreateStaticControl ( "SKILL_WINDOW_REST_POINT", pFont9, dwColor, TEXT_ALIGN_CENTER_X );		
        CString strTemp;
        strTemp.Format ( "0" );
        m_pRestPoint->AddText ( strTemp );
    }

    SetVisiblePage ( SKILL_WINDOW_PAGE0 );
}

CBasicTextButton*  CSkillWindow::CreateTextButton ( const char* szButton, UIGUID ControlID, const char* szText )
{
    const int nBUTTONSIZE = CBasicTextButton::SIZE19;
    CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
    pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ControlID );
    pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::RADIO_FLIP, szText );
    RegisterControl ( pTextButton );
    return pTextButton;
}

CSkillPage*	CSkillWindow::CreateSkillPage ( UIGUID ControlID, SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount )
{
    CSkillPage* pSkillPage = new CSkillPage ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
    pSkillPage->CreateSub ( this, "SKILL_PAGE", UI_FLAG_DEFAULT, ControlID );
    pSkillPage->CreateSubControl ( sNativeIDArray, nSkillCount );
    RegisterControl ( pSkillPage );
    return pSkillPage;
}

void CSkillWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

    switch ( ControlID )
    {
    case ET_CONTROL_TITLE:
    case ET_CONTROL_TITLE_F:
        {
            if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
            {
                m_pInterface->SetDefaultPosInterface( SKILL_WINDOW );
            }
        }
        break;
    case SKILL_WINDOW_PAGE_BUTTON0:
    case SKILL_WINDOW_PAGE_BUTTON1:
    case SKILL_WINDOW_PAGE_BUTTON2:	
    case SKILL_WINDOW_PAGE_BUTTON3:
        {
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg & UIMSG_LB_DOWN )
                {
                    int nIndex = ControlID - SKILL_WINDOW_PAGE_BUTTON0;
                    SetVisiblePage ( SKILL_WINDOW_PAGE0 + nIndex );
                }
            }
        }
        break;
    }

    if ( SKILL_WINDOW_PAGE0 <= ControlID && ControlID <= SKILL_WINDOW_PAGE3 )
    {
        int nIndex = ControlID - SKILL_WINDOW_PAGE0;
        int nSkillIndex = m_pPage[nIndex]->GetSkillIndex ();		

        if ( CHECK_MOUSE_IN ( dwMsg ) )
        {			
            if ( nSkillIndex < 0 ) return;
            SNATIVEID sNativeID = m_pPage[nIndex]->GetSkillID ( nSkillIndex );
            BOOL bView = m_pPage[nIndex]->GetViewSkillSlot ( nSkillIndex );
            if ( sNativeID != NATIVEID_NULL() && bView )
            {
                //				if ( CUIMan::GetFocusControl () == this )
                {
                    if ( dwMsg & UIMSG_MOUSEIN_BUTTON )
                    {
                        dwMsg &= ~UIMSG_TOTOPPARENT;
                        dwMsg &= ~UIMSG_MOUSEIN_BUTTON;
                        SetMessageEx ( dwMsg );

                        //	NOTE
                        //		레벨업을 했을때, 그 값이 바로 변경되어
                        //		출력되도록 하기 위해
                        //		메시지를 처리한것이다.
                        //		NATIVEID_NULL()을 날린것은,
                        //		의도적으로 이전ID와 다르게 하기 위함이다.
                        //		괜찮은 구조로 바꿔야한다.
                        if ( dwMsg & UIMSG_LEVEL_UP )
                        {							
                            dwMsg &= ~UIMSG_LEVEL_UP;
                            SetMessageEx ( dwMsg );
                            m_pInterface->RESET_INFO ();
                        }						

                        m_pInterface->SHOW_SKILL_INFO ( sNativeID, TRUE );
                        //						m_pInterface->UiShowGroupTop ( GENERAL_INFO_DISPLAY );
                    }
                    else
                    {
                        m_pInterface->SHOW_SKILL_INFO ( sNativeID, FALSE );
                        //						m_pInterface->UiShowGroupTop ( GENERAL_INFO_DISPLAY );
                    }
                }
            }
        }
    }
}

void CSkillWindow::GetSkillIndexRange ( EMCHARCLASS emCharClass, int* pnBeginIndex, int* pnEndIndex )
{	
    //	인덱스 결정
    int nBeginIndex = 0;
    int nEndIndex = 0;

    switch ( emCharClass )
    {
    case GLCC_FIGHTER_M:
    case GLCC_FIGHTER_W:
        {
            nBeginIndex = EMSKILL_FIGHTER_01;
            nEndIndex = EMSKILL_FIGHTER_04;
        }
        break;

    case GLCC_ARMS_M:
    case GLCC_ARMS_W:
        {
            nBeginIndex = EMSKILL_ARMS_01;
            nEndIndex = EMSKILL_ARMS_04;
        }
        break;

    case GLCC_ARCHER_M:
    case GLCC_ARCHER_W:
        {
            nBeginIndex = EMSKILL_ARCHER_01;
            nEndIndex = EMSKILL_ARCHER_04;
        }
        break;

    case GLCC_SPIRIT_M:
    case GLCC_SPIRIT_W:
        {
            nBeginIndex = EMSKILL_SPIRIT_01;
            nEndIndex = EMSKILL_SPIRIT_04;
        }
        break;	

    case GLCC_EXTREME_M:
    case GLCC_EXTREME_W:
        {
            nBeginIndex = EMSKILL_EXTREME_01;
            nEndIndex = EMSKILL_EXTREME_04;
        }
        break;
    case GLCC_SCIENTIST_M:
    case GLCC_SCIENTIST_W:
        {
            nBeginIndex = EMSKILL_SCIENTIST_01;
            nEndIndex = EMSKILL_SCIENTIST_04;			
        }
        break;
    case GLCC_ASSASSIN_M:
    case GLCC_ASSASSIN_W:
        {
            nBeginIndex = EMSKILL_ASSASSIN_01;
            nEndIndex = EMSKILL_ASSASSIN_04;			
        }
        break;
	case GLCC_TRICKER_M:
	case GLCC_TRICKER_W:
		{
			nBeginIndex = EMSKILL_TRICKER_01;
			nEndIndex = EMSKILL_TRICKER_04;			
		}
		break;
    case GLCC_ETC_M:
    case GLCC_ETC_W:
        {
            nBeginIndex = EMSKILL_ETC_01;
            nEndIndex = EMSKILL_ETC_04;			
        }
        break;
    };

    *pnBeginIndex = nBeginIndex;
    *pnEndIndex = nEndIndex;
}

void CSkillWindow::SortSkill ( EMCHARCLASS emCharClass, SNATIVEID sNativeIDArrayAll[EMSKILL_PC_CLASSNUM][GLSkillMan::MAX_CLASSSKILL], int nSkillCountAll[EMSKILL_PC_CLASSNUM] )
{
    int nBeginIndex = 0;
    int nEndIndex = 0;

    //	스킬 Index 가져오기
    GetSkillIndexRange ( emCharClass, &nBeginIndex, &nEndIndex );

    //	
    WORD wClass, wIndex;
    GLSkillMan::GetInstance().GetMaxSkill ( wClass, wIndex );

    for ( int i = nBeginIndex; i <= nEndIndex; i++ )
    {
        int nSkillCount = 0;
        SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL];
        for ( int k = 0; k < GLSkillMan::MAX_CLASSSKILL; k++ )
        {
            sNativeIDArray[k] = NATIVEID_NULL();
        }

        //	존재하는 스킬 뽑아오기
        for ( int j = 0; j < wIndex; j++ )
        {
            PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( i, j );
            if ( pSkill )
            {
                sNativeIDArray[nSkillCount] = SNATIVEID(i,j);
                nSkillCount++;
            }
        }

        //	그레이드별 정렬
        //	버블소트
        SNATIVEID sTempID;
        for ( int x = 0; x < nSkillCount; x++ )
            for ( int y = 0; y < nSkillCount; y++ )
            {
                PGLSKILL pA = GLSkillMan::GetInstance().GetData ( sNativeIDArray[x].wMainID, sNativeIDArray[x].wSubID );
                PGLSKILL pB = GLSkillMan::GetInstance().GetData ( sNativeIDArray[y].wMainID, sNativeIDArray[y].wSubID );
                if ( pA->m_sBASIC.dwGRADE < pB->m_sBASIC.dwGRADE )
                {
                    sTempID = sNativeIDArray[x];
                    sNativeIDArray[x] = sNativeIDArray[y];
                    sNativeIDArray[y] = sTempID;
                }
            }

            const int nIndex = i - nBeginIndex;
            nSkillCountAll[nIndex] = nSkillCount;
            memmove( sNativeIDArrayAll[nIndex], sNativeIDArray, sizeof ( SNATIVEID ) * GLSkillMan::MAX_CLASSSKILL );
    }
}

CUIControl*	CSkillWindow::CreateControl ( char* szControl )
{
    CUIControl* pControl = new CUIControl(m_pEngineDevice);
    pControl->CreateSub ( this, szControl );
    RegisterControl ( pControl );

    return pControl;
}


CBasicTextBox* CSkillWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
    CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
    pStaticText->CreateSub ( this, szControlKeyword );
    pStaticText->SetFont ( pFont );
    pStaticText->SetTextAlign ( nAlign );
    RegisterControl ( pStaticText );

    return pStaticText;
}

void CSkillWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible () ) return ;

    CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_pRestPoint )
    {
        const DWORD dwSkillPoint = m_pGaeaClient->GetCharacter ()->m_dwSkillPoint;
        if ( dwSkillPoint != m_dwSkillPointBACK )
        {
            CString strTemp;
            strTemp.Format ( "%d", dwSkillPoint );
            m_pRestPoint->SetText ( strTemp );
            m_dwSkillPointBACK = dwSkillPoint;
        }
    }
}

MySkillWindow::MySkillWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CSkillWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MySkillWindow::CreateUIWindowAndRegisterOwnership()
{
    CSkillWindow::Create( SKILL_WINDOW,  "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CSkillWindow::CreateBaseWindowLightGray( "SKILL_WINDOW", (char*)ID2GAMEWORD( "SKILL_NAME_STATIC" ) );
    CSkillWindow::CreateSubControl();
    CSkillWindow::SetControlNameEx( "스킬창" );
    CSkillWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    CSkillWindow::m_pInterface->UiRegisterControl( this, true );
    CSkillWindow::m_pInterface->UiShowGroupFocus( SKILL_WINDOW );
}

void MySkillWindow::ShowPageForTutorial()
{
    CSkillWindow::SetVisiblePageForTutorial();
}

const UIRECT& MySkillWindow::GetUIWindowGlobalPos()
{
    return CSkillWindow::GetGlobalPos();
}