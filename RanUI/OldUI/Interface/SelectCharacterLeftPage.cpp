#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"


#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../OuterInterface.h"
#include "SelectCharacterLeftPage.h"
#include "SelectCharacterCard.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCharacterLeftPage::CSelectCharacterLeftPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_pDelCharInfo(NULL)
    , m_pSelectCharacterCard(NULL)
    , m_pNewButton(NULL)
    , m_pServerText( NULL )
{
}

CSelectCharacterLeftPage::~CSelectCharacterLeftPage ()
{
}

void CSelectCharacterLeftPage::CreateSubControl ()
{
	//m_pSelectCharacterCard = new CSelectCharacterCard;
	//m_pSelectCharacterCard->CreateSub ( this, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	//m_pSelectCharacterCard->CreateBaseWidnow ( "SELECT_CHARACTER_LEFTWINDOW", (char*)ID2GAMEWORD("SELECT_CHARACTER_CARD") );
	//m_pSelectCharacterCard->CreateSubControl ();
	//RegisterControl ( m_pSelectCharacterCard );

	const int nBUTTONSIZE = CBasicTextButton::SIZE40;

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "SELECT_CHARACTER_LEFTBACK" );
		RegisterControl ( pBasicLineBox );		
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER_FAT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuterFat ( "SELECT_CHARACTER_SERVER_TEXT_BACK" );
		RegisterControl ( pBasicLineBox );		
	}


	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9 , _DEFAULT_FONT_FLAG );
		m_pServerText = CreateStaticControl ( "SELECT_CHARACTER_SERVER_TEXT", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	}


	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, SELECT_CHARACTER_DELETE );
		pButton->CreateBaseButton ( "SELECT_CHARACTER_DELETE", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 2 ) );
		pButton->SetShortcutKey ( DIK_DELETE );
		RegisterControl ( pButton );
	}

	{
		m_pNewButton = new CBasicTextButton(m_pEngineDevice);
		m_pNewButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, SELECT_CHARACTER_NEW );
		m_pNewButton->CreateBaseButton ( "SELECT_CHARACTER_NEW", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 0 ) );		
		RegisterControl ( m_pNewButton );
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, SELECT_CHARACTER_CHANGE_SERVER );
		pButton->CreateBaseButton ( "SELECT_CHARACTER_CHANGE_SERVER", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CHANGE_SERVER", 0 ) );
		pButton->SetShortcutKey ( DIK_ESCAPE );
		RegisterControl ( pButton );
	}

	
	{
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub ( this, "BASIC_TEXT_BUTTON40", UI_FLAG_XSIZE, SELECT_CHARACTER_GAME_START );
		pButton->CreateBaseButton ( "SELECT_CHARACTER_GAME_START", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 1 ) );
		RegisterControl ( pButton );
	}
}

CBasicTextBox* CSelectCharacterLeftPage::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

BOOL CSelectCharacterLeftPage::SndGameJoin(const char *psz2ndPass)
{
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
	// ������ ĳ���Ͱ� �����ϰ� Lock �� �ɸ��� ���� �����϶�
    if (pCharInfo && !pCharInfo->IsLock())
	{
		BOOL bNewPass = m_pGaeaClient->GetUse2ndPass() == 2 ? TRUE : FALSE;
		pGlobalStage->GetNetClient()->SndGameJoin(pCharInfo->m_dwCharID, psz2ndPass, bNewPass);

		m_pInterface->START_WAIT_TIME ( 60.0f );
		m_pInterface->DoModal(
            ID2GAMEEXTEXT ("CHARACTERSTAGE_GAME_JOIN"),
            UI::MODAL_INFOMATION,
            UI::CANCEL,
            OUTER_MODAL_CHARACTERSTAGE_GAME_JOIN_WAIT);
		return TRUE;
	}
	return FALSE;
}

void CSelectCharacterLeftPage::SndCharDelete()
{
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	m_pDelCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
	if (m_pDelCharInfo)
	{
		CString strTemp;					

		switch ( RENDERPARAM::emSERVICE_TYPE )
		{
		case RENDERPARAM::EMSERVICE_DEFAULT:		
		case RENDERPARAM::EMSERVICE_FEYA:		//Taiwan/Hongkong
		case RENDERPARAM::EMSERVICE_THAILAND:
		case RENDERPARAM::EMSERVICE_CHINA:
		case RENDERPARAM::EMSERVICE_INDONESIA:	//mjeon.indonesia.2�� �н����� ����
			strTemp = sc::string::format( ID2GAMEEXTEXT ("CHARACTERSTAGE_CAUTION_DELETE"), m_pDelCharInfo->m_ChaName.c_str() ).c_str();

			if( sp == SP_HONGKONG )
			{
				m_pInterface->DoModal ( strTemp.GetString(), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_SECONDPASSWORD, TRUE, TRUE, TRUE ); // 2����� �߰�����
			}
			else
			{
				m_pInterface->DoModal ( strTemp.GetString(), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_SECONDPASSWORD, TRUE );
			}

			break;

		default:			
			if( sp == SP_GS || sp == SP_KOREA || sp == SP_KOREA_TEST || sp == SP_OFFICE_TEST )
			{
				if( m_pGaeaClient->GetUse2ndPass() == 0 )
				{
					strTemp = m_pInterface->MakeString ( ID2GAMEEXTEXT ("CHARACTERSTAGE_CAUTION_DELETE_DAUM") );
					m_pInterface->DoModal ( strTemp.GetString(), UI::MODAL_QUESTION, UI::YESNO, OUTER_MODAL_SECONDPASSWORD );
				}
				else
				{
					strTemp = sc::string::format( ID2GAMEEXTEXT ("CHARACTERSTAGE_CAUTION_DELETE"), m_pDelCharInfo->m_ChaName.c_str() ).c_str();
					m_pInterface->DoModal ( strTemp.GetString(), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_SECONDPASSWORD, TRUE, TRUE, FALSE ); // 2����� �߰�����
				}
			}
			else
			{
				strTemp = m_pInterface->MakeString ( ID2GAMEEXTEXT ("CHARACTERSTAGE_CAUTION_DELETE_DAUM") );
				m_pInterface->DoModal ( strTemp.GetString(), UI::MODAL_QUESTION, UI::YESNO, OUTER_MODAL_SECONDPASSWORD );
			}
			break;
		};
	}
	else
	{
		m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_NOT_SELECTED"), UI::MODAL_INFOMATION, UI::OK );
	}
}

void CSelectCharacterLeftPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		AddMessageEx ( UIMSG_MOUSEIN_SELECT_CHARACTERLEFTPAGE );
	}

	switch ( ControlID )
	{
	case SELECT_CHARACTER_NEW:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				const USHORT uCharRemain = m_pInterface->GetCharRemain ();
				const int nChaSNum = pGlobalStage->GetLobyStage()->GetChaSNum ();

				//	������ ���������� ���� �ʰ�
				if ( MAX_ONESERVERCHAR_NUM <= nChaSNum )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTER_THIS_SERVER_SLOT_FULL" ), UI::MODAL_INFOMATION );
					break;
				}

				//	��밡��ī�尡 ���ڶ���
				if ( !uCharRemain )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTER_NOT_ENOUGH_CHARREMAIN" ), UI::MODAL_INFOMATION );
					break;
				}

				m_pInterface->ToCreateCharacterPage ( GetWndID () );
			}
		}
		break;
	case SELECT_CHARACTER_DELETE:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

				// ĳ�� ������ �븸�� ��� 2������� ����µ� �̶� ��������� �Ʒ� ĳ���� ������ �Ǵ� ���� ����
				// 2����� ����
				if( sp == SP_TAIWAN || sp == SP_KOREA || sp == SP_KOREA_TEST || sp == SP_GS || sp == SP_OFFICE_TEST )
				{
					// ĳ�� ������ 
					DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
					const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();

					// ĳ���� ���� ���� �� Lock ���� Ȯ��
					if (pCharInfo && !pCharInfo->IsLock())
					{		
						pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE; // TRUE�� ���� ������ ��������찡 ���� ĳ���Ͱ� Ŭ���ȴ�. �ϰ����� ���ؼ��� InnerMsg�� ������������ ������ ������ �̷��� �Ǿ��־ ����

						SndCharDelete();
					}
					else
					{
						pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE; 

						m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE, FALSE );
					}
				}
				else
				{
					SndCharDelete();
				}
			}
		}
		break;

	case SELECT_CHARACTER_CHANGE_SERVER:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_6"), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_LOGOUT );
			}
		}
		break;

	case SELECT_CHARACTER_GAME_START:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

				if( sp == SP_HONGKONG )
				{	
					// 2�����
					DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
					const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
					
					// ĳ���� ���� ���� �� Lock ���� Ȯ��
					if (pCharInfo && !pCharInfo->IsLock())
					{					
						pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE; // TRUE�� ���� ������ ��������찡 ���� ĳ���Ͱ� Ŭ���ȴ�. �ϰ����� ���ؼ��� InnerMsg�� ������������ ������ ������ �̷��� �Ǿ��־ ����

						m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_16"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_JOIN_SECOUNDPASSWORD, TRUE, TRUE );
					}
					else
					{
						pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE; 

						m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE, FALSE );
					}
				}
				else
				{
					// 2����� ��뿩�� Ȯ��
					// 2����� ������
					if( m_pGaeaClient->GetUse2ndPass() == 0 )
					{
						// �Ϲ� �α���
						if ( !SndGameJoin () )
						{
							m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK );
						}
					}
					else if( m_pGaeaClient->GetUse2ndPass() == 1 ) // 2����� ���
					{
						// �븸���� �ٸ� 2�����
						DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
						const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
						
						// ĳ���� ���� ���� �� Lock ���� Ȯ��
						if (pCharInfo && !pCharInfo->IsLock())
						{					
							pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE; // TRUE�� ���� ������ ��������찡 ���� ĳ���Ͱ� Ŭ���ȴ�. �ϰ����� ���ؼ��� InnerMsg�� ������������ ������ ������ �̷��� �Ǿ��־ ����

							m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_16"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
						}
						else
						{
							pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE; 

							m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE, FALSE );
						}
					}
					else if( m_pGaeaClient->GetUse2ndPass() == 2 ) // 2����� �űԼ��� �ʿ�
					{
						// �븸���� �ٸ� 2�����
						DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
						const SCHARINFO_LOBBY* pCharInfo = pGlobalStage->GetLobyStage()->GetSelectCharInfo();
						
						// ĳ���� ���� ���� �� Lock ���� Ȯ��
						if (pCharInfo && !pCharInfo->IsLock())
						{					
							pGlobalStage->GetLobyStage()->m_bGameJoin = TRUE; // TRUE�� ���� ������ ��������찡 ���� ĳ���Ͱ� Ŭ���ȴ�. �ϰ����� ���ؼ��� InnerMsg�� ������������ ������ ������ �̷��� �Ǿ��־ ����

							m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_20"), UI::MODAL_INPUT, UI::EDITBOX_CHECK_PWD, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
						}
						else
						{
							pGlobalStage->GetLobyStage()->m_bGameJoin = FALSE; 

							m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, FALSE, FALSE );
						}
					}
				}
			}
		}
		break;
	}
}

void CSelectCharacterLeftPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const USHORT uCharRemain = m_pInterface->GetCharRemain();
	CString strTemp;
	strTemp.Format ( "%s(%u)", (char*)ID2GAMEWORD ( "SELECT_CHARACTER_PAGE_BUTTON", 0 ), uCharRemain );
	m_pNewButton->SetOneLineText( strTemp );

	INT nServerInfo, nChannelInfo;
	CString strCombine;
	m_pInterface->GetConnectServerInfo( nServerInfo, nChannelInfo );
	strCombine.Format( _T( "%s - %d%s" ), ID2GAMEEXTEXT("SERVER_NAME",nServerInfo), nChannelInfo, ID2GAMEEXTEXT("SERVER_CHANNEL") );
	m_pServerText->SetOneLineText( strCombine );

	if ( UIKeyCheck::GetInstance()->Check ( DIK_ESCAPE, DXKEY_DOWN ) )
	{
		m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_6"), UI::MODAL_INFOMATION, UI::OKCANCEL, OUTER_MODAL_LOGOUT );
	}
	else if ( UIKeyCheck::GetInstance()->Check ( DIK_DELETE, DXKEY_DOWN ) )
	{
		SndCharDelete();
	}
}