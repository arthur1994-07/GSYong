#include "StdAfx.h"

#include "./QuestWindowRenewal.h"
#include "./List/QuestListRenewal.h"
#include "./Main/QuestMainRenewal.h"
#include "./Main/QuestMainEndRenewal.h"
#include "../../../../EngineLib/DeviceMan.h"
#include "../../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../InnerInterface.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../../RanLogic/Quest/GLQuest.h"
#include "../../Util/ModalWindow.h"
#include "../../../ModalCallerID.h"
#include "../QuestHelper.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/GUInterface/BasicLineBox.h"

// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CQuestWindowRenewal::CQuestWindowRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
    CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pQuestList(NULL)
	, m_pIngButton(NULL)
	, m_pDoneButton(NULL)
	, m_nPage(QUEST_WINDOW_PAGE_ING)
	, m_pQuestMain(NULL)
	, m_pQuestMainEnd(NULL)
	, m_pHelperCount(NULL)
{
}

CQuestWindowRenewal::~CQuestWindowRenewal()
{
}

void CQuestWindowRenewal::CreateSubControl()
{
	{
		CQuestListRenewal* pQuestList = new CQuestListRenewal( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pQuestList->CreateSub( this, "QUEST_WINDOW_LIST_PAGE_LUXES", UI_FLAG_DEFAULT, QUEST_LIST );
		pQuestList->CreateSubControl();
		RegisterControl( pQuestList );
		m_pQuestList = pQuestList;
	}

	{
		CQuestMainRenewal* pQuestMain = new CQuestMainRenewal( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pQuestMain->CreateSub( this, "QUEST_WINDOW_MORE_PAGE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN );
		pQuestMain->CreateSubControl();
		RegisterControl( pQuestMain );
		m_pQuestMain = pQuestMain;
	}

	{
		CQuestMainEndRenewal* pQuestMain = new CQuestMainEndRenewal( m_pGaeaClient, m_pInterface, m_pEngineDevice );
		pQuestMain->CreateSub( this, "QUEST_WINDOW_MORE_PAGE_LUXES", UI_FLAG_DEFAULT, QUEST_MAIN_END );
		pQuestMain->CreateSubControl();
		RegisterControl( pQuestMain );
		m_pQuestMainEnd = pQuestMain;
	}

	{
		const int nBUTTONSIZE = CBasicTextButton::SIZE19;
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, QUEST_ING_BUTTON );
		pButton->CreateBaseButton( "QUEST_WINDOW_ING_BUTTON_LUXES", nBUTTONSIZE, CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD("QUEST_LIST_BUTTON",0) );
		RegisterControl( pButton );
		m_pIngButton = pButton;
		m_pIngButton->SetFlip( FALSE );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, QUEST_DONE_BUTTON );
		pButton->CreateBaseButton( "QUEST_WINDOW_DONE_BUTTON_LUXES", nBUTTONSIZE, CBasicButton::RADIO_FLIP, (char*)ID2GAMEWORD("QUEST_LIST_BUTTON",1) );
		RegisterControl( pButton );
		m_pDoneButton = pButton;
		m_pDoneButton->SetFlip( FALSE );
	}

	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
		pTextBox->CreateSub( this, "QUEST_WINDOW_COUNTER_LUXES", UI_FLAG_DEFAULT, QUEST_HELPER_COUNT );
		pTextBox->SetFont( pFont9 );
		pTextBox->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y );
		RegisterControl( pTextBox );
		m_pHelperCount = pTextBox;
	}

	PageFlip( true );

	//m_pQuestList->LoadQuestList();
}

void CQuestWindowRenewal::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
		}
		break;

	case QUEST_ING_BUTTON:
	case QUEST_DONE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				bool bProg = (ControlID == QUEST_ING_BUTTON);
				PageFlip( bProg );

				// Memo : Page를 변경한다. by luxes.
			}
		}
		break;

	case QUEST_MAIN:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_BUTTON_GIVEUP_RENEWAL & dwMsg )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("QUEST_GIVEUP_CONFIRM"), MODAL_QUESTION, YESNO, UI::MODAL_QUEST_GIVEUP );
				}

				if ( UIMSG_BUTTON_COMPLETE_RENEWAL & dwMsg )
				{
					m_pGaeaClient->GetCharacter()->ReqQuestComplete( GetQuestMain()->GetQuestID() );
				}
			}
		}
		break;
	}
}

void CQuestWindowRenewal::PageFlip( bool bFilp )
{
	m_pIngButton->SetFlip( bFilp );
	m_pDoneButton->SetFlip( !bFilp );

	if ( bFilp )
	{
		m_nPage = QUEST_WINDOW_PAGE_ING;

		m_pQuestMain->SetVisibleSingle( TRUE );
		m_pQuestMainEnd->SetVisibleSingle( FALSE );
		m_pHelperCount->SetVisibleSingle( TRUE );
	}
	else
	{
		m_nPage = QUEST_WINDOW_PAGE_END;

		m_pQuestMain->SetVisibleSingle( FALSE );
		m_pQuestMainEnd->SetVisibleSingle( TRUE );
		m_pHelperCount->SetVisibleSingle( FALSE );
	}

	SET_SHOW_PAGE();
}

void CQuestWindowRenewal::REFRESH_QUEST_WINDOW()
{
	if ( !IsVisible() ) return ;
	LOAD_QUEST_LIST();
	LOAD_QUEST_MAIN();
}

void CQuestWindowRenewal::SetQuestID( DWORD dwQuestID )
{
	m_pQuestList->SetQuestID( dwQuestID );
}

void CQuestWindowRenewal::AUTO_LOAD_QUEST( DWORD dwQuestID )
{
	//if ( !IsVisible() ) return ;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc( dwQuestID );

	if ( !pQuestProg )
	{
		SET_SHOW_PAGE();
		return ;
	}

	LOAD_QUEST_LIST();
    if ( m_nPage == QUEST_WINDOW_PAGE_END )
        PageFlip( true );
	SetQuestID( dwQuestID );
	m_pQuestList->AutoLoadQuestList( dwQuestID );

	ShowHelperCount();
}

void CQuestWindowRenewal::AUTOSELECT_PAGE( DWORD dwQuestID )
{
	if ( !IsVisible() ) return ;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc( dwQuestID );

	if ( !pQuestProg )
	{
		SET_SHOW_PAGE();
		return ;
	}

	SetQuestID( dwQuestID );

	SET_SHOW_PAGE();
	/*const DWORD dwSTEP = pQuestProg->m_dwSTEP;	
	if ( dwSTEP < 1 )
		SET_SHOW_PAGE( );
	else
		SET_SHOW_PAGE( );*/
}

void CQuestWindowRenewal::SET_SHOW_PAGE()
{
	LOAD_QUEST_LIST();
	//LOAD_QUEST_MAIN();
	//LOAD_QUEST_STEP();

	ShowHelperCount();
}

bool CQuestWindowRenewal::LOAD_QUEST_LIST()
{
	return m_pQuestList->LoadQuestList();
}

bool CQuestWindowRenewal::LOAD_QUEST_MAIN()
{
	if ( GetPage() == 0 )
		return m_pQuestMain->LoadQuestMain( m_pQuestList->GetQuestIngID() );
	else if ( GetPage() == 1 )
		return m_pQuestMainEnd->LoadQuestMain( m_pQuestList->GetQuestEndID() );

	return false;
}

void CQuestWindowRenewal::ShowHelperCount()
{
	m_pHelperCount->ClearText();

	int nCurHelperNum = m_pInterface->GetQuestHelper()->GetHelperCount();
	int nMaxHelperNum = m_pInterface->GetQuestHelper()->GetHelperMaxCount();

	CString strCount;
	strCount.Format( ID2GAMEINTEXT( "QUEST_HELPER_COUNT" ), nCurHelperNum, nMaxHelperNum );

	m_pHelperCount->AddText( strCount );
}

const UIRECT& CQuestWindowRenewal::GetMainStepBackGP()
{
	return m_pQuestMain->GetStepBack()->GetGlobalPos();
}

bool CQuestWindowRenewal::CheckListAndReturnResult( DWORD dwID, bool bCurStatus )
{
    MULTIMAP_AREA_LIST& areaList = m_pQuestList->GetAreaList();

    MULTIMAP_AREA_ITER iter;

    for ( iter = areaList.begin(); iter != areaList.end(); iter++ )
    {
        SAREA_LIST& sArea = iter->second;
        MAP_QUEST_LIST& questList = sArea.GetQuestList();
        MAP_QUEST_ITER iter_quest = questList.find( dwID );

        if ( iter_quest != questList.end() )
        {
            if ( bCurStatus )
            {   // Uncheck.
                SQUEST_LIST& sQuest = iter_quest->second;
                sQuest.m_bCheck = false;
            }
            else
            {   // Check.
                SQUEST_LIST& sQuest = iter_quest->second;
                sQuest.m_bCheck = true;
            }

            return true;
        }
    }

    return false;
}

void CQuestWindowRenewal::SetQuestListID( DWORD dwID )
{
    m_pQuestList->SetQuestID( dwID );
}

bool CQuestWindowRenewal::OpenListAndReturnResult( DWORD dwID, bool bCurStatus )
{
    if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_ING )
    {
        MULTIMAP_AREA_LIST& areaList = m_pQuestList->GetAreaList();

        MULTIMAP_AREA_ITER iter = areaList.find( dwID );

        if ( iter != areaList.end() )
        {
            if ( bCurStatus )
            {   // Unopen.
                SAREA_LIST& sArea = iter->second;
                sArea.m_bOpen = false;
            }
            else
            {   // Open.
                SAREA_LIST& sArea = iter->second;
                sArea.m_bOpen = true;
            }

            m_pQuestList->LoadQuestList();

            return true;
        }
    }
    else if ( m_pInterface->GetQuestWindowRenewal()->GetPage() == CQuestWindowRenewal::QUEST_WINDOW_PAGE_END )
    {
        MULTIMAP_AREA_LIST& areaList = m_pQuestList->GetEndAreaList();

        MULTIMAP_AREA_ITER iter = areaList.find( dwID );

        if ( iter != areaList.end() )
        {
            if ( bCurStatus )
            {   // Unopen.
                SAREA_LIST& sArea = iter->second;
                sArea.m_bOpen = false;
            }
            else
            {   // Open.
                SAREA_LIST& sArea = iter->second;
                sArea.m_bOpen = true;
            }

            m_pQuestList->LoadQuestList();

            return true;
        }
    }

    m_pQuestList->LoadQuestList();
    return false;
}

const UIRECT& CQuestWindowRenewal::GetListPageGlobalPos()
{
    return m_pQuestList->GetGlobalPos();
}

DWORD CQuestWindowRenewal::GetQuestMainID()
{
    return m_pQuestMain->GetQuestID();
}

void CQuestWindowRenewal::LoadQuestMain( DWORD dwID )
{
    m_pQuestMain->LoadQuestMain( dwID );
}

void CQuestWindowRenewal::LoadQuestMainEnd( DWORD dwID )
{
    m_pQuestMainEnd->LoadQuestMain( dwID );
}

MyQuestWindowRenewal::MyQuestWindowRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CQuestWindowRenewal( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyQuestWindowRenewal::CreateUIWindowAndRegisterOwnership()
{
    CQuestWindowRenewal::Create( QUEST_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CQuestWindowRenewal::CreateBaseWindowLightGray ( "QUEST_WINDOW_LUXES", (char*)ID2GAMEWORD("QUEST_WINDOW_STATIC") );
    CQuestWindowRenewal::CreateSubControl();
    CQuestWindowRenewal::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );		
    CQuestWindowRenewal::m_pInterface->UiRegisterControl( this, true );
    CQuestWindowRenewal::m_pInterface->UiShowGroupFocus( QUEST_WINDOW );
}

void MyQuestWindowRenewal::AutoLoadQuest( DWORD dwQuestID )
{
    CQuestWindowRenewal::AUTO_LOAD_QUEST( dwQuestID );
}

UIGUID MyQuestWindowRenewal::GetWndID()
{
    return CQuestWindowRenewal::GetWndID();
}

void MyQuestWindowRenewal::RefreshQuestWindow()
{
    CQuestWindowRenewal::REFRESH_QUEST_WINDOW();
}

void MyQuestWindowRenewal::SetShowPage()
{
    CQuestWindowRenewal::SET_SHOW_PAGE();
}

void MyQuestWindowRenewal::AutoSelectPage( DWORD dwQuestID )
{
    CQuestWindowRenewal::AUTOSELECT_PAGE( dwQuestID );
}

const UIRECT& MyQuestWindowRenewal::GetUIWindowGlobalPos()
{
    return CQuestWindowRenewal::GetGlobalPos();
}

void MyQuestWindowRenewal::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CQuestWindowRenewal::SetGlobalPos( vPos );
}

const UIRECT& MyQuestWindowRenewal::GetUIWindowLocalPos()
{
    return CQuestWindowRenewal::GetLocalPos();
}

void MyQuestWindowRenewal::SetUIWindowLocalPos( const D3DXVECTOR2& vPos )
{
    CQuestWindowRenewal::SetLocalPos( vPos );
}

CQuestListRenewal* MyQuestWindowRenewal::GetQuestList()
{
    return CQuestWindowRenewal::GetQuestList();
}

CQuestMainRenewal* MyQuestWindowRenewal::GetQuestMain()
{
    return CQuestWindowRenewal::GetQuestMain();
}

BOOL MyQuestWindowRenewal::IsVisible()
{
    return CQuestWindowRenewal::IsVisible();
}

const UIRECT& MyQuestWindowRenewal::GetMainStepBackPos()
{
    return CQuestWindowRenewal::GetMainStepBackGP();
}

int MyQuestWindowRenewal::GetPage()
{
    return CQuestWindowRenewal::GetPage();
}

CQuestMainEndRenewal* MyQuestWindowRenewal::GetQuestMainEnd()
{
    return CQuestWindowRenewal::GetQuestMainEnd();
}

void MyQuestWindowRenewal::ShowHelperCount()
{
    CQuestWindowRenewal::ShowHelperCount();
}

bool MyQuestWindowRenewal::CheckListAndReturnResult( DWORD dwID, bool bCurStatus )
{
    return CQuestWindowRenewal::CheckListAndReturnResult( dwID, bCurStatus );
}

void MyQuestWindowRenewal::SetQuestListID( DWORD dwID )
{
    CQuestWindowRenewal::SetQuestListID( dwID );
}
bool MyQuestWindowRenewal::OpenListAndReturnResult( DWORD dwID, bool bCurStatus )
{
    return CQuestWindowRenewal::OpenListAndReturnResult( dwID, bCurStatus );
}

const UIRECT& MyQuestWindowRenewal::GetListPageGlobalPos()
{
    return CQuestWindowRenewal::GetListPageGlobalPos();
}

DWORD MyQuestWindowRenewal::GetQuestMainID()
{
    return CQuestWindowRenewal::GetQuestMainID();
}

void MyQuestWindowRenewal::LoadQuestMain( DWORD dwID )
{
    CQuestWindowRenewal::LoadQuestMain( dwID );
}

void MyQuestWindowRenewal::LoadQuestMainEnd( DWORD dwID )
{
    CQuestWindowRenewal::LoadQuestMainEnd( dwID );
}
