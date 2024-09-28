#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Club/GLClubClient.h"

#include "../../InnerInterface.h"
#include "../Interface/BasicChat.h"
#include "ClubWindow.h"
#include "ClubBattle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CClubWindow::nLIMIT_CHAR = 16;

CClubWindow::CClubWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pClubMember(NULL)
	, m_bClubMaster(false)
	, m_pMemberButton(NULL)
	, m_pAnnouncementButton(NULL)
	, m_pUnionButton(NULL)
	, m_pClubAnnounce(NULL)
	, m_pClubUnion(NULL)
	, m_pLeaveButton(NULL)
	, m_pPartyButton(NULL)
	, m_pAuthorityButton(NULL)
	, m_pWriteButton(NULL)
	, m_pSubMasterButton(NULL)
	, m_nPageMode(CLUB_ANNOUNCEMENT_PAGE)
	, m_pWriteButtonOK(NULL)
	, m_pWriteButtonCancel(NULL)
	, m_pWriteButtonClear(NULL)
	, m_bEditBox(FALSE)
	, m_pUnionKickButton(NULL)
	, m_pUnionLeaveButton(NULL)
	, m_pUnionBreakupButton(NULL)
    , m_pBattleButton(NULL) 
	, m_pBattleArmisticeButton(NULL)
	, m_pBattleSubmissionButton(NULL)
	, m_pClubBattle ( NULL ) 
	, m_pBattlePoint ( NULL )
	, m_pAllianceBattlePoint( NULL ) 
{
}

CClubWindow::~CClubWindow()
{
}

CBasicTextBox* CClubWindow::CreateTextBox(char* szKeyword, CD3DFontPar* pFont)
{
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub(this, szKeyword);
	pTextBox->SetFont(pFont);
	RegisterControl(pTextBox);
	return pTextBox;
}

void CClubWindow::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont(_DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG);	
	CBasicTextBox* pTextBox = NULL;	

	{	//	뒷 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub(this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
		pBasicLineBox->CreateBaseBoxQuestList("CLUB_UP_LINEBOX");
		RegisterControl(pBasicLineBox);		
	}

	{	//	클럽이름
		pTextBox = CreateTextBox("CLUB_NAME_STATIC", pFont);
		pTextBox->AddText(ID2GAMEWORD("CLUB_TEXT_STATIC",0));
		m_pClubName = CreateTextBox("CLUB_NAME_TEXTBOX", pFont);
	}

	{	//	클럽등급
		pTextBox = CreateTextBox("CLUB_GRADE_STATIC", pFont);
		pTextBox->AddText(ID2GAMEWORD("CLUB_TEXT_STATIC",1));
		m_pClubGrade = CreateTextBox("CLUB_GRADE_TEXTBOX", pFont);
	}

	{	//	마스터 이름
		pTextBox = CreateTextBox("CLUB_MASTER_NAME_STATIC", pFont);
		pTextBox->AddText(ID2GAMEWORD("CLUB_TEXT_STATIC",2));
		m_pMasterName = CreateTextBox("CLUB_MASTER_NAME_TEXTBOX", pFont);
	}

	{	//	창설일
		pTextBox = CreateTextBox("CLUB_ESTABLISH_DATE_STATIC", pFont);
		pTextBox->AddText(ID2GAMEWORD("CLUB_TEXT_STATIC",3));
		m_pEstablishDateStatic = pTextBox;
		m_pEstablishDate = CreateTextBox("CLUB_ESTABLISH_DATE_TEXTBOX", pFont);
	}

	{	//	클럽 마크
		pTextBox = CreateTextBox("CLUB_MARK_STATIC", pFont);
		pTextBox->AddText(ID2GAMEWORD("CLUB_TEXT_STATIC",4));

		//	CUICONTROL 제어
		m_pClubMark = CreateControl("CLUB_WINDOW_MARK");		
		m_pClubMark->SetUseRender(TRUE);
	}

	{
		pTextBox = CreateTextBox ( "CLUB_NICK_STATIC", pFont );
		pTextBox->AddText ( ID2GAMEWORD("CLUB_TEXT_STATIC",6) );		

		m_pNickName = CreateTextBox ( "CLUB_NICK_TEXTBOX", pFont );
	}
	
	// 클럽배틀
	if ( GLCONST_CHAR::bCLUB_BATTLE )
	{
		//	전적
		pTextBox = CreateTextBox ( "CLUB_BATTLE_POINT_STATIC", pFont );
		pTextBox->AddText ( ID2GAMEWORD("CLUB_TEXT_STATIC",9) );

		if ( GLCONST_CHAR::bCLUB_BATTLE_ALLIANCE )
		{
			//	전적
			pTextBox = CreateTextBox ( "CLUB_ALLIANCE_BATTLE_POINT_STATIC", pFont );
			pTextBox->AddText ( ID2GAMEWORD("CLUB_TEXT_STATIC",13) );
		}
	}

	m_pBattlePoint = CreateTextBox ( "CLUB_BATTLE_POINT_TEXTBOX", pFont );
	m_pAllianceBattlePoint = CreateTextBox ( "CLUB_ALLIANCE_BATTLE_POINT_TEXTBOX", pFont );

	/////////////// 탭 버튼 /////////////////
	
	if ( GLCONST_CHAR::bCLUB_BATTLE )
	{
		m_pAnnouncementButton = CreateTextButton18( "CLUB_ANNOUNCEMENT_BUTTON_BATTLE", CLUB_ANNOUNCEMENT_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",8)), TRUE );
		m_pMemberButton = CreateTextButton18( "CLUB_MEMBER_BUTTON_BATTLE", CLUB_MEMBER_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",7)), FALSE );
		m_pUnionButton = CreateTextButton18( "CLUB_UNION_BUTTON_BATTLE", CLUB_UNION_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",9)), FALSE );	
	}
	else
	{
		m_pAnnouncementButton = CreateTextButton18( "CLUB_ANNOUNCEMENT_BUTTON", CLUB_ANNOUNCEMENT_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",8)), TRUE );
		m_pMemberButton = CreateTextButton18( "CLUB_MEMBER_BUTTON", CLUB_MEMBER_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",7)), FALSE );
		m_pUnionButton = CreateTextButton18( "CLUB_UNION_BUTTON", CLUB_UNION_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",9)), FALSE );
	}

	m_pBattleButton = CreateTextButton18( "CLUB_BATTLE_BUTTON_BATTLE", CLUB_BATTLE_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",13)), FALSE );
	/////////////////////////////////////////

	/////////////////// 탭 페이지 ////////////////////
	CClubAnnounce* pClubAnnounce = new CClubAnnounce(m_pGaeaClient, m_pEngineDevice);
	pClubAnnounce->CreateSub( this, "CLUBWINDOW_SHEET", UI_FLAG_DEFAULT, CLUB_ANNOUNCEMENT_PAGE );
	pClubAnnounce->CreateSubControl();
	RegisterControl( pClubAnnounce );
	m_pClubAnnounce = pClubAnnounce;

	CClubMember* pClubMember = new CClubMember(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pClubMember->CreateSub(this, "CLUBWINDOW_SHEET", UI_FLAG_DEFAULT, CLUB_MEMBER_PAGE);
	pClubMember->CreateSubControl();
	RegisterControl(pClubMember);
	m_pClubMember = pClubMember;

	CClubUnion* pClubUnion = new CClubUnion(m_pGaeaClient, m_pEngineDevice);
	pClubUnion->CreateSub( this, "CLUBWINDOW_SHEET", UI_FLAG_DEFAULT, CLUB_UNION_PAGE );
	pClubUnion->CreateSubControl();
	RegisterControl( pClubUnion );
	m_pClubUnion = pClubUnion;

	CClubBattle* pClubBattle = new CClubBattle(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pClubBattle->CreateSub( this, "CLUBWINDOW_SHEET", UI_FLAG_DEFAULT, CLUB_BATTLE_PAGE );
	pClubBattle->CreateSubControl();
	RegisterControl( pClubBattle );
	m_pClubBattle = pClubBattle;

	//////////////////////////////////////////////////

	m_pKickButton = CreateTextButton14( "CLUB_KICK_BUTTON", CLUB_KICK_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",0)) );
	m_pLeaveButton = CreateTextButton14( "CLUB_LEAVE_BUTTON", CLUB_LEAVE_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",6)) );
	m_pPartyButton = CreateTextButton14( "CLUB_PARTY_BUTTON", CLUB_PARTY_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",17)) );
	m_pAuthorityButton = CreateTextButton14( "CLUB_AUTHORITY_BUTTON", CLUB_AUTHORITY_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",16)) );
	m_pWriteButton = CreateTextButton14( "CLUB_WRITE_BUTTON", CLUB_WRITE_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",10)) );
	m_pWriteButtonOK = CreateTextButton14( "CLUB_WRITE_OK_BUTTON", CLUB_WRITE_OK_BUTTON, const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_MAKE_BUTTON",0)) );
	m_pWriteButtonCancel = CreateTextButton14( "CLUB_WRITE_CANCEL_BUTTON", CLUB_WRITE_CANCEL_BUTTON, const_cast<char*>(ID2GAMEWORD("PRIVATE_MARKET_MAKE_BUTTON",1)) );
	m_pWriteButtonClear = CreateTextButton14( "CLUB_WRITE_CLEAR_BUTTON", CLUB_WRITE_CLEAR_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",12)) );
	m_pSubMasterButton = CreateTextButton14( "CLUB_SUBMASTER_BUTTON", CLUB_SUBMASTER_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",11)) );
	m_pBreakupButton = CreateTextButton14( "CLUB_BREAKUP_BUTTON", CLUB_BREAKUP_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",1)) );
	m_pChangeMarkButton = CreateTextButton14( "CLUB_CHANGE_MARK_BUTTON", CLUB_CHANGE_MARK_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",2)) );
	m_pChangeNickButton = CreateTextButton14( "CLUB_CHANGE_TITLE_BUTTON", CLUB_CHANGE_TITLE_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",4)) );

	m_pUnionKickButton = CreateTextButton14( "CLUB_UNION_KICK_BUTTON", CLUB_UNION_KICK_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",0)) );
	m_pUnionLeaveButton = CreateTextButton14( "CLUB_UNION_LEAVE_BUTTON", CLUB_UNION_LEAVE_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",6)) );
	m_pUnionBreakupButton = CreateTextButton14( "CLUB_UNION_BREAKUP_BUTTON", CLUB_UNION_BREAKUP_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",1)) );

	m_pBattleArmisticeButton = CreateTextButton14( "CLUB_BATTLE_ARMISTICE_BUTTON", CLUB_BATTLE_ARMISTICE_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",14)) );
	m_pBattleSubmissionButton = CreateTextButton14( "CLUB_BATTLE_SUBMISSION_BUTTON", CLUB_BATTLE_SUBMISSION_BUTTON, const_cast<char*>(ID2GAMEWORD("CLUB_BUTTON",15)) );
	

	

}

CBasicTextButton* CClubWindow::CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText )
{
	GASSERT( szButton && szText);

	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );

	return pButton;
}

CBasicTextButton* CClubWindow::CreateTextButton18( char* szButton, UIGUID ControlID, char* szText, BOOL bFlip )
{
	GASSERT( szButton && szText);

	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::RADIO_FLIP, szText );
	pButton->SetFlip ( bFlip );
	RegisterControl ( pButton );

	return pButton;
}

void CClubWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( CLUB_WINDOW );
			}
		}
		break;
	case CLUB_ANNOUNCEMENT_BUTTON:
	case CLUB_MEMBER_BUTTON:
	case CLUB_UNION_BUTTON:
	case CLUB_BATTLE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetVisiblePage ( cID );
			}
		}
		break;	

	case CLUB_KICK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( !m_pClubMember->IsSelectedIndex() ) return;				

				CString strName = GetClubMemberName ();

				std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_KICK_MEMBER"), strName.GetString() );
				m_pInterface->DoModal ( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_KICK );
			}
		}
		break;

	case CLUB_LEAVE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();

				//	길드원이 아닌 경우
				if (!pMyClub->IsValid())
                    return;

				m_pInterface->DoModal ( ID2GAMEINTEXT("CLUB_SELFQUIT"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_SELFQUIT );
			}
		}
		break;

	case CLUB_PARTY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( !m_pClubMember->IsSelectedIndex() )
                    return;	

				m_pClubMember->PartyRecruit();
			}  

		}
		break;

	case CLUB_AUTHORITY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( !m_pClubMember->IsSelectedIndex() )
                    return;				

				CString strName = GetClubMemberName();
				std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_AUTHORITY_MEMBER"), strName.GetString() );
				m_pInterface->DoModal ( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_AUTHORITY );
			}
		}
		break;

	case CLUB_WRITE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pClubAnnounce->BeginEdit();
				m_pWriteButton->SetVisibleSingle( FALSE );
				m_pWriteButtonOK->SetVisibleSingle( TRUE );
				m_pWriteButtonCancel->SetVisibleSingle( TRUE );
				m_pWriteButtonClear->SetVisibleSingle( TRUE );
				m_bEditBox = TRUE;
			}
		}
		break;

	case CLUB_WRITE_OK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pClubAnnounce->EndEdit( TRUE );
				m_pWriteButton->SetVisibleSingle( TRUE );
				m_pWriteButtonOK->SetVisibleSingle( FALSE );
				m_pWriteButtonCancel->SetVisibleSingle( FALSE );
				m_pWriteButtonClear->SetVisibleSingle( FALSE );
				m_bEditBox = FALSE;
			}
		}
		break;

	case CLUB_WRITE_CANCEL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pClubAnnounce->EndEdit( FALSE );
				m_pWriteButton->SetVisibleSingle( TRUE );
				m_pWriteButtonOK->SetVisibleSingle( FALSE );
				m_pWriteButtonCancel->SetVisibleSingle( FALSE );
				m_pWriteButtonClear->SetVisibleSingle( FALSE );
				m_bEditBox = FALSE;
			}
		}
		break;

	case CLUB_WRITE_CLEAR_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pClubAnnounce->ClearText();
			}
		}
		break;

	case CLUB_SUBMASTER_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if( !m_pClubMember->IsSelectedIndex() )
                    return;

				GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
				DWORD dwCharID = GetClubMemberID();
				if (pMyClub->IsMaster(dwCharID))
                    return; // 마스터이면 그냥 리턴

				AddMessageEx( UIMSG_MOUSEIN_SUBMASTER_MAKE );
			}
		}
		break;

	case CLUB_BREAKUP_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
				if (!pMyClub->IsRegDissolution())
				{
					m_pInterface->DoModal ( ID2GAMEINTEXT("CLUB_BREAKUP"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_BREAKUP );
				}
				else
				{
					m_pInterface->DoModal ( ID2GAMEINTEXT("CLUB_BREAKUP_CANCEL"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_BREAKUP_CANCEL );
				}			
			}
		}
		break;

	case CLUB_CHANGE_MARK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal ( ID2GAMEINTEXT("CLUB_CHANGE_MARK"), UI::MODAL_INPUT, UI::MODAL_FILE_NAME, UI::MODAL_CLUB_CHANGE_MARK );
			}
		}
		break;

	case CLUB_CHANGE_TITLE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();

				//	길드원이 아닌 경우
				if (!pMyClub->IsValid())
                    return;

				m_pInterface->DoModal ( ID2GAMEINTEXT("CLUB_CHANGE_NICK"), UI::MODAL_INPUT, UI::EDITBOX, UI::MODAL_CLUB_NICK );
			}
		}
		break;

	case CLUB_UNION_KICK_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( !m_pClubUnion->IsSelectedIndex() )
                    return;				

				CString strName = GetUnionMemberName();
				std::string strCombine	= sc::string::format( ID2GAMEINTEXT("UNION_KICK_MEMBER"), strName.GetString() );
				m_pInterface->DoModal ( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_UNION_KICK );
			}
		}
		break;

	case CLUB_UNION_LEAVE_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal( ID2GAMEINTEXT("UNION_LEAVE"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_UNION_LEAVE );
			}
		}
		break;

	case CLUB_UNION_BREAKUP_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal ( ID2GAMEINTEXT("UNION_BREAKUP"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_UNION_BREAKUP );
			}
		}
		break;
	case CLUB_BATTLE_ARMISTICE_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( !m_pClubBattle->IsSelectedIndex() )
                    return;				

				CString strName = GetBattleClubName();
				bool bAlliance = GetIsAllianceBattle();
				
				if( bAlliance ) 
				{
					std::string strCombine = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_ARMISTICE"), strName.GetString() );
					m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_ALLIANCE_BATTLE_ARMISTICE );
				}
				else
				{
					std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_ARMISTICE"), strName.GetString() );
					m_pInterface->DoModal( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_BATTLE_ARMISTICE );
				}

			}
		}
		break;

	case CLUB_BATTLE_SUBMISSION_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( !m_pClubBattle->IsSelectedIndex() )
                    return;				

				CString strName = GetBattleClubName();
				bool bAlliance = GetIsAllianceBattle();
			
				if ( bAlliance )
				{
					std::string strCombine = sc::string::format( ID2GAMEINTEXT("ALLIANCE_BATTLE_SUBMISSION"), strName.GetString() );
					m_pInterface->DoModal ( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_ALLIANCE_BATTLE_SUBMISSION );
				}
				else
				{
					std::string strCombine = sc::string::format( ID2GAMEINTEXT("CLUB_BATTLE_SUBMISSION"), strName.GetString() );
					m_pInterface->DoModal ( strCombine, UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_CLUB_BATTLE_SUBMISSION );
				}
			}
		}
		break;
	
	}
}

void CClubWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		//m_pGaeaClient->ReqClubInfoUpdate( true );
		RefreshState ();

		// 클럽배틀
		if ( !GLCONST_CHAR::bCLUB_BATTLE )
		{
			m_pBattleButton->SetVisibleSingle( FALSE );
			m_pClubBattle->SetVisibleSingle( FALSE );
			m_pBattlePoint->SetVisibleSingle( FALSE );
		}
		else if ( !GLCONST_CHAR::bCLUB_BATTLE_ALLIANCE ) 
		{
			m_pAllianceBattlePoint->SetVisibleSingle( FALSE );
		}
	}
	else
	{
		m_pClubAnnounce->EndEdit( FALSE );
		m_bEditBox = FALSE;

		//m_pGaeaClient->ReqClubInfoUpdate( false );
	}
}

void CClubWindow::ResetState ()
{
	m_pKickButton->SetVisibleSingle ( FALSE );
	m_pLeaveButton->SetVisibleSingle( FALSE );
	m_pPartyButton->SetVisibleSingle( FALSE );
	m_pAuthorityButton->SetVisibleSingle ( FALSE );
	m_pWriteButton->SetVisibleSingle( FALSE );
	m_pWriteButtonOK->SetVisibleSingle( FALSE );
	m_pWriteButtonCancel->SetVisibleSingle( FALSE );
	m_pWriteButtonClear->SetVisibleSingle( FALSE );
	m_pSubMasterButton->SetVisibleSingle( FALSE );
	m_pBreakupButton->SetVisibleSingle ( FALSE );
	m_pChangeMarkButton->SetVisibleSingle ( FALSE );
	m_pChangeNickButton->SetVisibleSingle ( FALSE );

	m_pUnionKickButton->SetVisibleSingle( FALSE );
	m_pUnionLeaveButton->SetVisibleSingle( FALSE );
	m_pUnionBreakupButton->SetVisibleSingle( FALSE );

	m_pBattleArmisticeButton->SetVisibleSingle( FALSE );
	m_pBattleSubmissionButton->SetVisibleSingle( FALSE );

	m_pClubName->ClearText ();
	m_pClubGrade->ClearText ();
	m_pMasterName->ClearText ();
	m_pEstablishDate->ClearText ();
	m_pEstablishDateStatic->SetOneLineText ( ID2GAMEWORD("CLUB_TEXT_STATIC",3) );
	m_pNickName->ClearText ();
	m_pBattlePoint->ClearText ();
	m_pAllianceBattlePoint->ClearText ();

	m_pMemberButton->SetFlip( FALSE );
	m_pAnnouncementButton->SetFlip( FALSE );
	m_pUnionButton->SetFlip( FALSE );
	m_pBattleButton->SetFlip( FALSE );

	m_pClubAnnounce->SetVisibleSingle( FALSE );
	m_pClubMember->SetVisibleSingle( FALSE );
	m_pClubUnion->SetVisibleSingle( FALSE );
	m_pClubBattle->SetVisibleSingle( FALSE );
}

void CClubWindow::RefreshState ()
{
	ResetState ();

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	DWORD dwCharID = m_pGaeaClient->GetCharacter()->m_CharDbNum;

	//	길드원이 아닌 경우
	if ( !pMyClub->IsMember( dwCharID ) )
        return;

	m_bClubMaster = pMyClub->IsMaster( dwCharID );
	
	//	이름
	m_pClubName->SetOneLineText ( pMyClub->Name(), NS_UITEXTCOLOR::GREENYELLOW );
	
	//	등급		
	m_pClubGrade->SetOneLineText ( ID2GAMEWORD("CLUB_GRADE",pMyClub->m_dwRank), NS_UITEXTCOLOR::YELLOW );
	
	//	마스터 이름
	m_pMasterName->SetOneLineText ( pMyClub->GetMasterName(), NS_UITEXTCOLOR::YELLOW );
	
	//	창설일/해산일
	if ( pMyClub->IsRegDissolution () )	//	해산
	{
		RefreshTime (pMyClub->m_tDissolution);
		m_pBreakupButton->SetOneLineText ( ID2GAMEWORD("CLUB_BUTTON",5) );
		m_pEstablishDateStatic->SetOneLineText ( ID2GAMEWORD("CLUB_TEXT_STATIC",5) );
	}
	else
	{
		RefreshTime (pMyClub->m_tOrganize);
		m_pBreakupButton->SetOneLineText ( ID2GAMEWORD("CLUB_BUTTON",1) );
		m_pEstablishDateStatic->SetOneLineText ( ID2GAMEWORD("CLUB_TEXT_STATIC",3) );
	}		

	m_pNickName->SetOneLineText ( m_pGaeaClient->GetCharacter()->m_szNick );
	m_pChangeNickButton->SetVisibleSingle ( TRUE );

	CString strBattle;
	strBattle.Format ( "%u%s %u%s %u%s", pMyClub->m_dwBattleWin, ID2GAMEWORD("CLUB_TEXT_STATIC",10),
						pMyClub->m_dwBattleLose, ID2GAMEWORD("CLUB_TEXT_STATIC",11), pMyClub->m_dwBattleDraw,
						ID2GAMEWORD("CLUB_TEXT_STATIC",12) );

	m_pBattlePoint->SetOneLineText( strBattle );

	if ( pMyClub->IsAlliance() )
	{
		strBattle.Format ( "%u%s %u%s %u%s", pMyClub->m_dwAllianceBattleWin, ID2GAMEWORD("CLUB_TEXT_STATIC",10),
							pMyClub->m_dwAllianceBattleLose, ID2GAMEWORD("CLUB_TEXT_STATIC",11), 
							pMyClub->m_dwAllianceBattleDraw, ID2GAMEWORD("CLUB_TEXT_STATIC",12) );

		m_pAllianceBattlePoint->SetOneLineText( strBattle );
	}


	
	CUIControl* pVisiblePage = NULL;
	//CBasicTextButton* pVisibleButton = NULL;
	CBasicTextButton* pFlipButton = NULL;

	switch( m_nPageMode )
	{
	case CLUB_ANNOUNCEMENT_PAGE:
		{
			if( pMyClub->IsMemberFlgNotice(dwCharID) )
			{
				//pVisibleButton = m_pWriteButton;
				m_pWriteButton->SetVisibleSingle( TRUE );
			}

			pVisiblePage = m_pClubAnnounce;
			pFlipButton = m_pAnnouncementButton;
		}
		break;

	case CLUB_MEMBER_PAGE:
		{
			if( pMyClub->IsMaster( dwCharID ) )
			{
				//pVisibleButton = m_pKickButton;
				m_pKickButton->SetVisibleSingle( TRUE );
				m_pAuthorityButton->SetVisibleSingle( TRUE );
				m_pSubMasterButton->SetVisibleSingle( TRUE );
			}
			else if( pMyClub->HaveMemberKickAuthority(dwCharID) )
			{
				//pVisibleButton = m_pKickButton;
				m_pKickButton->SetVisibleSingle( TRUE );
				m_pLeaveButton->SetVisibleSingle( TRUE );
			}
			else
			{
				//pVisibleButton = m_pLeaveButton;
				m_pLeaveButton->SetVisibleSingle( TRUE );
			}

			m_pPartyButton->SetVisibleSingle( TRUE );

			pVisiblePage = m_pClubMember;
			pFlipButton = m_pMemberButton;
		}
		break;

	case CLUB_UNION_PAGE:
		{
			pVisiblePage = m_pClubUnion;
			pFlipButton = m_pUnionButton;

			//if( 동맹장 )
			if( pMyClub->IsChief() )
			{
				m_pUnionKickButton->SetVisibleSingle( TRUE );
				m_pUnionBreakupButton->SetVisibleSingle( TRUE );
			}
			else if( pMyClub->IsMaster( dwCharID ) && pMyClub->IsAlliance() )
			{
				m_pUnionLeaveButton->SetVisibleSingle( TRUE );
			}
		}
		break;
	case CLUB_BATTLE_PAGE:
		{
			pVisiblePage = m_pClubBattle;
			pFlipButton = m_pBattleButton;

			if ( pMyClub->IsMaster( dwCharID ) )
			{
				m_pBattleArmisticeButton->SetVisibleSingle( TRUE );
				m_pBattleSubmissionButton->SetVisibleSingle( TRUE );
			}
		}
		break;
	}

	//if( NULL != pVisibleButton ) pVisibleButton->SetVisibleSingle( TRUE );
	if( NULL != pVisiblePage )
        pVisiblePage->SetVisibleSingle( TRUE );
	if( NULL != pFlipButton )
        pFlipButton->SetFlip( TRUE );

	if( m_bClubMaster )
        m_pBreakupButton->SetVisibleSingle( TRUE ); // 클럽해체버튼

	if( pMyClub->IsMemberFlgMarkChange(dwCharID) )
	{
		m_pChangeMarkButton->SetVisibleSingle( TRUE ); // 마크 변경 버튼
	}
}

void CClubWindow::RefreshTime ( const __time64_t& tTime )
{
	CTime cTime (tTime);

	//	서버 타임으로 변환( TimeZone 계산 )
	m_pGaeaClient->GetConvertServerTime( cTime );

	CString strTemp;

	strTemp.Format ( "%04d/%02d/%02d %02d:%02d",
		cTime.GetYear (), cTime.GetMonth (), cTime.GetDay (),
		cTime.GetHour (), cTime.GetMinute () );

	m_pEstablishDate->SetOneLineText ( strTemp, NS_UITEXTCOLOR::DARKORANGE );	
}

HRESULT CClubWindow::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	if ( !IsVisible () )
        return S_OK;

	HRESULT hr = S_OK;

	m_pClubMark->SetVisibleSingle ( FALSE );
	{
		hr = CUIWindowEx::Render ( pd3dDevice );
		if( FAILED ( hr ) )
            return hr;
	}
	m_pClubMark->SetVisibleSingle ( TRUE );

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();

	//	길드원이 아닌 경우
	if (!pMyClub->IsValid()) 
        return S_OK;

	int nServer = static_cast<int>(m_pGaeaClient->GetCharacter()->m_dwServerID);
	int nID = static_cast<int>(pMyClub->DbNum());
	DWORD dwVer = pMyClub->m_dwMarkVER;

	//if ( !DxClubMan::GetInstance().IsValidData ( nServer, nID, dwVer ) ) return S_OK;
	const clubmark::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData ( pd3dDevice, nServer, nID );

	//const UIRECT& rcGlobalPos = m_pClubMark->GetGlobalPos ();
	//m_pClubMark->SetGlobalPos ( UIRECT ( ceil(rcGlobalPos.left), ceil(rcGlobalPos.top),
	//	ceil(rcGlobalPos.sizeX), ceil(rcGlobalPos.sizeY) ) );

	m_pClubMark->SetTexturePos ( 0, sMarkData.vTex_1_LU );
	m_pClubMark->SetTexturePos ( 1, sMarkData.vTex_2_RU );
	m_pClubMark->SetTexturePos ( 3, sMarkData.vTex_3_LD );
	m_pClubMark->SetTexturePos ( 2, sMarkData.vTex_4_RD );

	//	만들어야함
	m_pClubMark->SetTexture ( sMarkData.pddsTexture );
	{
		hr = m_pClubMark->Render ( pd3dDevice );
		if( FAILED ( hr ) )
            return hr;
	}

	return S_OK;
}

void CClubWindow::SetVisiblePage( UIGUID cID )
{
	if ( cID < CLUB_MEMBER_BUTTON || CLUB_BATTLE_BUTTON < cID )
	{
		GASSERT ( 0 && "영역을 넘어서는 페이지입니다." );
		return;
	}

	GLClubClient* pMyClub = m_pGaeaClient->GetMyClub();
	DWORD dwCharID = m_pGaeaClient->GetCharacter()->m_CharDbNum;

	//	길드원이 아닌 경우
	if (!pMyClub->IsMember(dwCharID))
        return;

	CUIControl* pVisiblePage = NULL;
	CBasicTextButton* pVisibleButton = NULL;

	{	//	리셋
		m_pClubMember->SetVisibleSingle( FALSE );
		m_pClubAnnounce->SetVisibleSingle( FALSE );
		m_pClubUnion->SetVisibleSingle( FALSE );
		m_pClubBattle->SetVisibleSingle( FALSE );
	
		m_pMemberButton->SetFlip( FALSE );
		m_pAnnouncementButton->SetFlip( FALSE );
		m_pUnionButton->SetFlip( FALSE );
		m_pBattleButton->SetFlip( FALSE );

		m_pKickButton->SetVisibleSingle( FALSE );
		m_pLeaveButton->SetVisibleSingle( FALSE );
		m_pPartyButton->SetVisibleSingle( FALSE );
		m_pAuthorityButton->SetVisibleSingle( FALSE );		
		m_pWriteButton->SetVisibleSingle( FALSE );
		m_pWriteButtonOK->SetVisibleSingle( FALSE );
		m_pWriteButtonCancel->SetVisibleSingle( FALSE );
		m_pWriteButtonClear->SetVisibleSingle( FALSE );
		m_pSubMasterButton->SetVisibleSingle( FALSE );

		m_pUnionKickButton->SetVisibleSingle( FALSE );
		m_pUnionLeaveButton->SetVisibleSingle( FALSE );
		m_pUnionBreakupButton->SetVisibleSingle( FALSE );

		m_pBattleArmisticeButton->SetVisibleSingle( FALSE );
		m_pBattleSubmissionButton->SetVisibleSingle( FALSE );
	}

	switch ( cID )
	{
	case CLUB_ANNOUNCEMENT_BUTTON:
		{
			pVisiblePage = m_pClubAnnounce;
			pVisibleButton = m_pAnnouncementButton;
			m_nPageMode = CLUB_ANNOUNCEMENT_PAGE;

			if( pMyClub->IsMemberFlgNotice(dwCharID) )
			{
				if( m_bEditBox )
				{
					m_pWriteButtonOK->SetVisibleSingle( TRUE );
					m_pWriteButtonCancel->SetVisibleSingle( TRUE );
					m_pWriteButtonClear->SetVisibleSingle( TRUE );
				}
				else
				{
					m_pWriteButton->SetVisibleSingle( TRUE );
				}
			}
		}
		break;

	case CLUB_MEMBER_BUTTON:
		{
			pVisiblePage = m_pClubMember;
			pVisibleButton = m_pMemberButton;
			m_nPageMode = CLUB_MEMBER_PAGE;

			if( pMyClub->IsMaster( dwCharID ) )
			{
				m_pAuthorityButton->SetVisibleSingle( TRUE );
				m_pKickButton->SetVisibleSingle( TRUE );
				m_pSubMasterButton->SetVisibleSingle( TRUE );
			}
			else if( pMyClub->HaveMemberKickAuthority(dwCharID) )
			{
				m_pKickButton->SetVisibleSingle( TRUE );
				m_pLeaveButton->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pLeaveButton->SetVisibleSingle( TRUE );
			}

			m_pPartyButton->SetVisibleSingle ( TRUE );
		}
		break;

	case CLUB_UNION_BUTTON:
		{
			pVisiblePage = m_pClubUnion;
			pVisibleButton = m_pUnionButton;
			m_nPageMode = CLUB_UNION_PAGE;

			if( pMyClub->IsChief() )
			{
				m_pUnionKickButton->SetVisibleSingle( TRUE );
				m_pUnionBreakupButton->SetVisibleSingle( TRUE );
			}
			else if( pMyClub->IsMaster( dwCharID ) && pMyClub->IsAlliance() )
			{
				m_pUnionLeaveButton->SetVisibleSingle( TRUE );
			}
		}
		break;
	case CLUB_BATTLE_BUTTON:
		{
			pVisiblePage = m_pClubBattle;
			pVisibleButton = m_pBattleButton;
			m_nPageMode = CLUB_BATTLE_PAGE;

			if ( pMyClub->IsMaster( dwCharID ) )
			{
				m_pBattleArmisticeButton->SetVisibleSingle( TRUE );
				m_pBattleSubmissionButton->SetVisibleSingle( TRUE );
			}
		}
	}

	pVisiblePage->SetVisibleSingle ( TRUE );
	pVisibleButton->SetFlip ( TRUE );
}

MyClubWindow::MyClubWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CClubWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyClubWindow::CreateUIWindowAndRegisterOwnership()
{
    Create( CLUB_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CreateBaseWindowBlack( "CLUB_WINDOW", (char*)ID2GAMEWORD("CLUB_WINDOW_NAME_STATIC") );
    CreateSubControl();
    SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );		
    m_pInterface->UiRegisterControl( this, true );
    m_pInterface->UiShowGroupFocus( GetWndID() );
}

void MyClubWindow::LoadClubAllianceList()
{
    CClubWindow::LoadClubAllianceList();
}

void MyClubWindow::LoadClubBattleList()
{
    CClubWindow::LoadClubBattleList();
}

void MyClubWindow::RefreshState()
{
    CClubWindow::RefreshState();
}

void MyClubWindow::SetAnnouncement()
{
    CClubWindow::SetAnnouncement();
}

const DWORD MyClubWindow::GetClubMemberID()
{
    return CClubWindow::GetClubMemberID();
}

const DWORD MyClubWindow::GetBattleClubID()
{
    return CClubWindow::GetBattleClubID();
}

const DWORD MyClubWindow::GetUnionMemberID()
{
    return CClubWindow::GetUnionMemberID();
}

const UI::String MyClubWindow::GetClubMemberName()
{
    return UI::ToString( CClubWindow::GetClubMemberName() );
}

void MyClubWindow::RefreshClubList()
{
    CClubWindow::LoadClubMemberList();
}
