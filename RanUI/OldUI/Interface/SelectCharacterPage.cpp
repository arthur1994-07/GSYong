#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/Stage/DxLobyStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "SelectCharacterPage.h"
#include "SelectCharacterInfoPage.h"
#include "SelectCharacterPageList.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCharacterPage::CSelectCharacterPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pInterface(pInterface)
    , m_pPageNumTextBox(NULL)
    , m_pStartButton(NULL)
{
}

CSelectCharacterPage::~CSelectCharacterPage ()
{
}

void CSelectCharacterPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CSelectCharacterInfoPage* pSelectCharacterPage = new CSelectCharacterInfoPage ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
	pSelectCharacterPage->CreateSub ( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pSelectCharacterPage->CreateBaseWidnow ( "SELECT_CHARACTER_UPWINDOW", (char*)ID2GAMEWORD("CREATE_CHARACTER_PAGE", 1 ) );
	pSelectCharacterPage->CreateSubControl ();
	RegisterControl ( pSelectCharacterPage );
	m_pSelectCharacterPage = pSelectCharacterPage;

	CSelectCharacterPageList* pSelectCharacterPageList = new CSelectCharacterPageList(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	pSelectCharacterPageList->CreateSub ( this, "SELECT_CHAR_WINDOW_CHAR_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE , SELECT_CHARACTER_LIST );
	//pSelectCharacterPageList->CreateBaseWidnow ( "SELECT_CHAR_WINDOW_CHAR_LIST", (char*)ID2GAMEWORD("SELECT_CHARACTER_PAGE") );
	pSelectCharacterPageList->CreateSubControl ();
	RegisterControl ( pSelectCharacterPageList );
	m_pSelectCharacterPageList = pSelectCharacterPageList;


	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
}

BOOL CSelectCharacterPage::SndGameJoin()
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
	
    // ������ ĳ���Ͱ� �����ϰ� Lock �� �ɸ��� ���� ����
    if (pCharInfo && !pCharInfo->IsLock())
	{   
		SndGameJoinMsg();
		return TRUE;
	}
    else
    {
        pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE;
        return FALSE;
    }
}

bool CSelectCharacterPage::CheckSelectClubCondition( const SCHARINFO_LOBBY* pCharInfo )
{
    if ( !pCharInfo )
        return false;

    if ( GLUseFeatures::GetInstance().IsUsingEssentialSelectClub() )
    {
        if ( pCharInfo->m_ClubDbNum == 0 )
            return true;
        else
            return false;
    }
    else
    {
        if ( pCharInfo->m_wLevel == 1 && pCharInfo->m_ClubDbNum == 0 )
            return true;
        else
            return false;
    }

    return false;
}

void CSelectCharacterPage::SndGameJoinMsg()
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();

    if (pCharInfo && !pCharInfo->IsLock())
    {   // ������ ĳ���Ͱ� �����ϰ� Lock �� �ɸ��� ���� ����.
        pGlobalStage->GetNetClient()->SndGameJoin(pCharInfo->m_dwCharID);

        m_pInterface->START_WAIT_TIME( 60.0f );
		
		EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
		
		// 2�����
		if( sp == SP_HONGKONG )
		{
			m_pInterface->DoModal( ID2GAMEEXTEXT("CHARACTERSTAGE_GAME_JOIN"), UI::MODAL_INFOMATION, UI::CANCEL, OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT, FALSE, TRUE ); 
		}
		else
		{
			// 2�����
			// GS, KR���� 2����� ����
			if( m_pGaeaClient->GetUse2ndPass() == 0 )
			{
				m_pInterface->DoModal( ID2GAMEEXTEXT("CHARACTERSTAGE_GAME_JOIN"), UI::MODAL_INFOMATION, UI::CANCEL, OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT, FALSE, FALSE );
			}
			else if( m_pGaeaClient->GetUse2ndPass() == 1 ||  m_pGaeaClient->GetUse2ndPass() == 2 )
			{
				m_pInterface->DoModal( ID2GAMEEXTEXT("CHARACTERSTAGE_GAME_JOIN"), UI::MODAL_INFOMATION, UI::CANCEL, OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT, FALSE, TRUE ); 
			}
		}	
    }
    else
    {
        pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE;
    }
}

void CSelectCharacterPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		AddMessageEx ( UIMSG_MOUSEIN_SELECT_CHARACTERPAGE );
	}

	switch ( ControlID )
	{
	case SELECT_CHARACTER_START:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();

				if( m_pGaeaClient->GetUse2ndPass() == 0 )
				{
					//	NOTE
					//		����
					if ( !SndGameJoin () )
					{
						//m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE );
						m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK );
					}
				}
				else if( m_pGaeaClient->GetUse2ndPass() == 1 || m_pGaeaClient->GetUse2ndPass() == 2 ) // 2����� ���
				{
					const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();

					// ĳ���� ���� ���� �� Lock ���� Ȯ��
					if (pCharInfo && !pCharInfo->IsLock())
					{
						pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE; // TRUE�� ���� ������ ��������찡 ���� ĳ���Ͱ� Ŭ���ȴ�. �ϰ����� ���ؼ��� InnerMsg�� ������������ ������ ������ �̷��� �Ǿ��־ ����

						if( m_pGaeaClient->GetUse2ndPass() == 1 )
							m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_16"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
						else
							m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_20"), UI::MODAL_INPUT, UI::EDITBOX_CHECK_PWD, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
					}
					else
					{
						pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE; 
						m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE, FALSE );
					}
				}
				else
				{
					pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE; 
					m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_UNKNOWN_ERROR_2ND_PWD"), UI::MODAL_INFOMATION, UI::OK, FALSE, FALSE );
				}
			}
		}
		break;

	case SELECT_CHARACTER_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_6"), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_LOGOUT );
			}
		}
		break;
	}
}

void CSelectCharacterPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSelectCharacterPage::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		UIRECT rcLocalPos = m_pSelectCharacterPage->GetLocalPos();
		UIRECT rcPos = GetGlobalPos();

		rcLocalPos.top = rcPos.bottom + 10;

		m_pSelectCharacterPage->SetGlobalPos( rcLocalPos );
	}
    else
    {
        m_pInterface->UiHideGroup( CHARACTER_COUNTRY_CLUB_INFO );
    }
}

void CSelectCharacterPage::ResetAll ()
{
	if ( m_pSelectCharacterPage ) 
		m_pSelectCharacterPage->ResetAll ();

	if( m_pSelectCharacterPageList )
		m_pSelectCharacterPageList->ResetAll();
}