#include "StdAfx.h"
#include "QuestWindow.h"
#include "QuestList.h"
#include "QuestMain.h"
#include "QuestStep.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/Quest/GLQuestMan.h"
#include "../../../RanLogic/Quest/GLQuest.h"
//#include "../../ModalCallerID.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../InnerInterface.h"

#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h" // by luxes.

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CQuestWindow::CQuestWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_nPage(QUEST_LIST)
    , m_dwQuestID(NATIVEID_NULL().dwID)
{
}

CQuestWindow::~CQuestWindow ()
{
}

void	CQuestWindow::CreateSubControl()
{
	CQuestList* pQuestList = new CQuestList( m_pGaeaClient, m_pEngineDevice );
	pQuestList->CreateSub( this, "QUEST_LIST", UI_FLAG_DEFAULT, QUEST_LIST );
	pQuestList->CreateSubControl();
	RegisterControl( pQuestList );
	m_pQuestList = pQuestList;

	CQuestMain* pQuestStep = new CQuestMain( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pQuestStep->CreateSub( this, "QUEST_STEP", UI_FLAG_DEFAULT, QUEST_MAIN );
	pQuestStep->CreateSubControl();
	RegisterControl( pQuestStep );
	m_pQuestMain = pQuestStep;

	CQuestStep* pQuestRun = new CQuestStep( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pQuestRun->CreateSub( this, "QUEST_RUN", UI_FLAG_DEFAULT, QUEST_STEP );
	pQuestRun->CreateSubControl();
	RegisterControl( pQuestRun );
	m_pQuestStep = pQuestRun;

	m_pMoreButton = CreateTextButton14( "QUEST_LIST_BUTTON_MORE", QUEST_LIST_BUTTON_MORE, (char*)ID2GAMEWORD("QUEST_LIST_BUTTON",2) );	

	SET_SHOW_PAGE( QUEST_LIST );
}

void CQuestWindow::SET_SHOW_PAGE( int nPage, bool bMakeMsg )
{
//	if ( bMakeMsg && !IsVisible () ) return ;

	if ( bMakeMsg )
	{
		//m_ActionMsgQ.PostUIMessage( QUEST_LIST, UIMSG_INVISIBLE );		
		//m_ActionMsgQ.PostUIMessage( QUEST_MAIN, UIMSG_INVISIBLE );
		//m_ActionMsgQ.PostUIMessage( QUEST_STEP, UIMSG_INVISIBLE );
		m_pQuestList->SetVisibleSingle( FALSE );
		m_pQuestMain->SetVisibleSingle( FALSE );
		m_pQuestStep->SetVisibleSingle( FALSE );
		m_pMoreButton->SetVisibleSingle( FALSE );
	}
	else
	{
		m_pQuestList->SetVisibleSingle( FALSE );
		m_pQuestMain->SetVisibleSingle( FALSE );
		m_pQuestStep->SetVisibleSingle( FALSE );
		m_pMoreButton->SetVisibleSingle( FALSE );
	}

	switch ( nPage )
	{
	case QUEST_LIST:
		{
			if ( !LOAD_QUEST_LIST() )
			{
				GASSERT ( 0 && "오류, 퀘스트 정보를 찾을 수 없습니다." );
				return ;
			}

			if ( bMakeMsg )
			{
				//m_ActionMsgQ.PostUIMessage( QUEST_LIST, UIMSG_VISIBLE );
				m_pQuestList->SetVisibleSingle( TRUE );
				m_pMoreButton->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pQuestList->SetVisibleSingle( TRUE );
				m_pMoreButton->SetVisibleSingle( TRUE );
			}
		}
		break;

	case QUEST_MAIN:
		{
			if ( !LOAD_QUEST_MAIN() )
			{				
				SET_SHOW_PAGE( QUEST_LIST, bMakeMsg );
				return ;
			}

			//if ( bMakeMsg )
			//{
				//m_ActionMsgQ.PostUIMessage( QUEST_MAIN, UIMSG_VISIBLE );
			//	m_pQuestMain->SetVisibleSingle( TRUE );
			//}
			//else
			//{
				m_pQuestMain->SetVisibleSingle( TRUE );
			//}

			if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
			{
				if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
					&& m_pGaeaClient->GetTutorial()->IsSubTwoStep())
				{
					m_pGaeaClient->GetTutorial()->NextSubStep();
				}
			}
		}
		break;

	case QUEST_STEP:
		{
			if ( !LOAD_QUEST_STEP() )
			{				
				SET_SHOW_PAGE( QUEST_LIST , bMakeMsg );
				return ;
			}

			// if else equal
            //if ( bMakeMsg )	//m_ActionMsgQ.PostUIMessage( QUEST_STEP, UIMSG_VISIBLE );
                m_pQuestStep->SetVisibleSingle( TRUE );  
			//else
            //    m_pQuestStep->SetVisibleSingle( TRUE );

			if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
			{
				if ( m_pGaeaClient->GetTutorial()->IsQuestWindowStep()
					&& m_pGaeaClient->GetTutorial()->IsSubSixStep() )
				{
					m_pGaeaClient->GetTutorial()->NextSubStep();
				}
			}
		}
		break;
	default: GASSERT ( 0 && "알 수 없는 페이지입니다." );	  break;
	}

	m_nPage = nPage;
}


void	CQuestWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( QUEST_WINDOW );
			}
		}
		break;
	case QUEST_LIST:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					DWORD dwQuestID = m_pQuestList->GetQuestID();
					if ( dwQuestID == NATIVEID_NULL().dwID ) return ;

					SetQuestID( dwQuestID );
				}

				if ( UIMSG_LIST_LB_DUP & dwMsg )
				{
					SET_SHOW_PAGE( QUEST_MAIN, true );
				}
			}
		}
		break;

	case QUEST_MAIN:
		{
			if ( CHECK_MOUSE_IN( dwMsg ) )
			{
				if ( UIMSG_BUTTON_GIVEUP & dwMsg )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("QUEST_GIVEUP_CONFIRM"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_QUEST_GIVEUP );
				}

				if ( UIMSG_BUTTON_GO_QUESTSTEP & dwMsg )
				{
					SET_SHOW_PAGE( QUEST_STEP, true );
				}

				if ( UIMSG_BUTTON_BACK & dwMsg )
				{
					SET_SHOW_PAGE( QUEST_LIST, true );
				}

				if ( UIMSG_BUTTON_COMPLETE & dwMsg )
				{
					m_pGaeaClient->GetCharacter()->ReqQuestComplete( GetQuestID() );
				}
			}
		}
		break;

	case QUEST_STEP:
		{
			if ( UIMSG_BUTTON_BACK_LBUPLIKE & dwMsg )
			{
				SET_SHOW_PAGE( QUEST_MAIN, true );
			}

			if ( UIMSG_BUTTON_COMPLETE_STEP & dwMsg )
			{
				m_pGaeaClient->GetCharacter()->ReqQuestComplete( GetQuestID() );
			}
			
			if ( UIMSG_BUTTON_PRINT_STEP & dwMsg )
			{
				m_pInterface->SET_QUEST_HELPER( GetQuestID() );	
			}
		}
		break;

	case QUEST_LIST_BUTTON_MORE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{				
				SET_SHOW_PAGE( QUEST_MAIN, true );
			}
		}
		break;
	}
}

bool	CQuestWindow::LOAD_QUEST_LIST()
{	
	SetQuestID( NATIVEID_NULL().dwID );
	SetTitleName( ID2GAMEWORD("QUEST_WINDOW_STATIC") );	
	return m_pQuestList->LoadQuestList();
}

bool	CQuestWindow::LOAD_QUEST_MAIN()
{
	DWORD dwQuestID = GetQuestID();
	if ( dwQuestID == SNATIVEID(false).dwID ) return false;

	if ( !m_pQuestMain->LoadQuestMain( dwQuestID ) )
	{
		//GASSERT ( 0 && "QuestMain Load Failed!" );
		return false;
	}

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
	if ( !pQuest ) return false;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
		SetTitleName( (char*)CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );
	}
	else
	{
		SetTitleName( (char*)pQuest->GetTITLE() );
	}

	return true;
}

bool	CQuestWindow::LOAD_QUEST_STEP()
{
	DWORD dwQuestID = GetQuestID();
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	if ( !m_pQuestStep->LoadQuestStep( dwQuestID ) )
	{
		//GASSERT ( 0 && "QuestStep Load Failed!" );
		return false;
	}

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwQuestID );
	if ( !pQuest ) return false;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc( dwQuestID );
	if ( !pQuestProg ) return false;

	DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP( dwSTEP );		
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, dwSTEP);
		SetTitleName( CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString() );	
	}
	else
	{
		SetTitleName( pQuestStep->GetTITLE() );	
	}

	return true;
}

void CQuestWindow::AUTOSELECT_PAGE( DWORD dwQuestID )
{
	if ( !IsVisible () ) return ;

	GLQuestPlay& cQuestPlay = m_pGaeaClient->GetCharacter()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc( dwQuestID );
	if ( !pQuestProg )
	{
		SET_SHOW_PAGE( QUEST_LIST );
		return ;
	}

	SetQuestID( dwQuestID );
	const DWORD dwSTEP = pQuestProg->m_dwSTEP;	
	if ( dwSTEP < 1 )
		SET_SHOW_PAGE( QUEST_MAIN );
	else
		SET_SHOW_PAGE( QUEST_STEP );
}

void	CQuestWindow::REFRESH_QUEST_WINDOW()
{	
	if ( !IsVisible() ) return ;
	const int nPage = GET_SHOW_PAGE();
	SET_SHOW_PAGE( nPage, true );
}

CBasicTextButton* CQuestWindow::CreateTextButton14( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pButton );
	return pButton;
}

const UIRECT& CQuestWindow::GetProgListTextBoxGlobalPos()
{
    return m_pQuestList->GetListTexProg()->GetGlobalPos();
}

const UIRECT& CQuestWindow::GetProgListTextBoxLocalPos()
{
    return m_pQuestList->GetListTexProg()->GetLocalPos();
}

const UIRECT& CQuestWindow::GetProgMainTextBoxGlobalPos()
{
    return m_pQuestMain->GetLineBox()->GetGlobalPos();
}

const UIRECT& CQuestWindow::GetProgMainTextBoxLocalPos()
{
    return m_pQuestMain->GetLineBox()->GetLocalPos();
}

const UIRECT& CQuestWindow::GetProgMainMoreBtnGlobalPos()
{
    return m_pQuestMain->GetMoreButton()->GetGlobalPos();
}

MyQuestWindow::MyQuestWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CQuestWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyQuestWindow::CreateUIWindowAndRegisterOwnership()
{
    CQuestWindow::Create( QUEST_WINDOW_TEMP, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CQuestWindow::CreateBaseWindowBlack( "QUEST_WINDOW", (char*)ID2GAMEWORD("QUEST_WINDOW_STATIC") );
    CQuestWindow::CreateSubControl();
    CQuestWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );		
    CQuestWindow::m_pInterface->UiRegisterControl( this, true );
    CQuestWindow::m_pInterface->UiShowGroupFocus( QUEST_WINDOW_TEMP );
}

UIGUID MyQuestWindow::GetWndID()
{
    return CQuestWindow::GetWndID();
}

void MyQuestWindow::RefreshQuestWindow()
{
    CQuestWindow::REFRESH_QUEST_WINDOW();
}

int MyQuestWindow::GetShowPage()
{
    return CQuestWindow::GET_SHOW_PAGE();
}

DWORD MyQuestWindow::GetQuestID()
{
    return CQuestWindow::GetQuestID();
}

void MyQuestWindow::SetShowPage( int nPage, bool bMakeMsg /*= false */ )
{
    CQuestWindow::SET_SHOW_PAGE( nPage, bMakeMsg );
}

void MyQuestWindow::AutoSelectPage( DWORD dwQuestID )
{
    CQuestWindow::AUTOSELECT_PAGE( dwQuestID );
}

const UIRECT& MyQuestWindow::GetUIWindowGlobalPos()
{
    return CQuestWindow::GetGlobalPos();
}

void MyQuestWindow::SetUIWindowGlobalPos( const D3DXVECTOR2& vPos )
{
    CQuestWindow::SetGlobalPos( vPos );
}

const UIRECT& MyQuestWindow::GetProgListTextBoxGlobalPos()
{
    return CQuestWindow::GetProgListTextBoxGlobalPos();
}

const UIRECT& MyQuestWindow::GetProgListTextBoxLocalPos()
{
    return CQuestWindow::GetProgListTextBoxLocalPos();
}

const UIRECT& MyQuestWindow::GetProgMainTextBoxGlobalPos()
{
    return CQuestWindow::GetProgMainTextBoxGlobalPos();
}

const UIRECT& MyQuestWindow::GetProgMainTextBoxLocalPos()
{
    return CQuestWindow::GetProgMainTextBoxLocalPos();
}

const UIRECT& MyQuestWindow::GetProgMainMoreBtnGlobalPos()
{
    return CQuestWindow::GetProgMainMoreBtnGlobalPos();
}