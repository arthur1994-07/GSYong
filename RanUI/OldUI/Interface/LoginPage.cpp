#include "StdAfx.h"
#include "../../../SigmaCore/DebugSet.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtils.h"

#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/DxResponseMan.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"


#include "../../../RanLogicClient/Network/s_NetClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../OuterInterface.h"
#include "LoginPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int	CLoginPage::nLIMIT_ID = 12;
int	CLoginPage::nLIMIT_PW = 16;
int	CLoginPage::nLIMIT_RP = 6;

CLoginPage::CLoginPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIOuterWindow(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
	, m_pIDSaveButton(NULL)
	, m_pRandTextBox(NULL)
	, m_nRPUpdateCnt(0)
{
}

CLoginPage::~CLoginPage ()
{
}

void CLoginPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nBUTTONSIZE = CBasicTextButton::SIZE19;

	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "LOGIN_PAGE_ID", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "LOGIN_PAGE_IDPW", 0 ) );
	pTextBox = CreateStaticControl ( "LOGIN_PAGE_PW", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "LOGIN_PAGE_IDPW", 1 ) );

	// 랜덤키 추가%%%
//#if defined TW_PARAM || defined HK_PARAM || defined _RELEASED
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	if (sp == SP_OFFICE_TEST || sp == SP_HONGKONG)
	{
		// Note : 랜덤넘버 안내문
		m_pRandTextBox = CreateStaticControl ( "LOGIN_PAGE_RP", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	}
//#endif
	
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "LOGIN_PAGE_ID_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "LOGIN_PAGE_PW_BACK" );
		RegisterControl ( pBasicLineBox );

		// 랜덤키 추가%%%
//#if defined TW_PARAM || defined HK_PARAM || defined _RELEASED
		if (sp == SP_OFFICE_TEST || sp == SP_HONGKONG)
		{
			pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxOuter ( "LOGIN_PAGE_RP_BACK" );
			RegisterControl ( pBasicLineBox );
		}
//#endif
	}

	{
		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
		pEditBoxMan->CreateSub ( this, "LOGIN_EDITMAN", UI_FLAG_DEFAULT, LOGIN_EDIT_MAN );
		pEditBoxMan->CreateEditBox ( LOGIN_EDIT_ID, "LOGIN_EDIT_ID", "LOGIN_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_ID );
		pEditBoxMan->CreateEditBox ( LOGIN_EDIT_PW, "LOGIN_EDIT_PW", "LOGIN_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW );
		// 랜덤키 추가%%%
//#if defined TW_PARAM || defined HK_PARAM || defined _RELEASED
		if (sp == SP_OFFICE_TEST || sp == SP_HONGKONG)
		{
			pEditBoxMan->CreateEditBox ( LOGIN_EDIT_RP, "LOGIN_EDIT_RP", "LOGIN_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_RP );
			pEditBoxMan->SetHide ( LOGIN_EDIT_RP, TRUE );
		}
//#endif
		pEditBoxMan->SetHide ( LOGIN_EDIT_PW, TRUE );
		RegisterControl( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;
	}

	{
		m_pIDSaveButton = CreateFlipButton ( "LOGIN_PAGE_IDSAVE_BUTTON", "LOGIN_PAGE_IDSAVE_BUTTON_F", LOGIN_PAGE_IDSAVE_BUTTON );
		pTextBox = CreateStaticControl ( "LOGIN_PAGE_IDSAVE_BACK", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
		pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "LOGIN_PAGE_IDSAVE_BACK" ) );
	}

	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, LOGIN_OK );
	pOKButton->CreateBaseButton ( "LOGIN_PAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "LOGIN_PAGE_OKCANCEL", 0 ) );
	pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( pOKButton );

	CBasicTextButton* pQuitButton = new CBasicTextButton(m_pEngineDevice);
	pQuitButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, LOGIN_CANCEL );
	pQuitButton->CreateBaseButton ( "LOGIN_PAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "LOGIN_PAGE_OKCANCEL", 1 ) );
	pQuitButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pQuitButton );

	//CreateControl ( "LOGIN_BOTTOM" );

	LoadIDSaveButton ();
}	

void CLoginPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( !pNetClient->IsOnline() )		//네트워크 연결이 끊어진 경우
	{
		if ( !m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			if( !m_pInterface->IsLoginCancel() )
			{
				m_pInterface->DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_1"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_RECONNECT );
			}
		}
	}
	else
	{
		// 랜덤키 추가%%%
//#if defined TW_PARAM || defined HK_PARAM || defined _RELEASED
		EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
		if (sp == SP_OFFICE_TEST || sp == SP_HONGKONG)
		{
			int nRandPassNumber = pNetClient->GetRandomPassNumber();

			if( nRandPassNumber != 0 )
			{
				CString strTemp;
				strTemp = sc::string::format( ID2GAMEEXTEXT( "LOGIN_PAGE_RAND_NUMBER" ), nRandPassNumber ).c_str();
				if( m_pRandTextBox ) m_pRandTextBox->SetOneLineText( strTemp );
			}
			else
			{
				if( m_nRPUpdateCnt++ > 10 )
				{
					pNetClient->SndRequestRandomKey();
					m_nRPUpdateCnt = 0;
				}
			}
		}
//#endif
	}

	//	탭 이동
	if ( m_pEditBoxMan )
	{
		CUIControl* pParent = m_pEditBoxMan->GetTopParent ();
		if ( !pParent )	pParent = this;	//	만약 이클래스가 최상위 컨트롤인 경우
		BOOL bFocus = ( pParent->IsFocusControl() );

		if ( bFocus )
		{
			if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
			{
				m_pEditBoxMan->GoNextTab ();
			}
		}
	}
}

void CLoginPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	//
	//mjeon.indonesia
	//
	EMSERVICE_PROVIDER sp =	m_pGaeaClient->GetServiceProvider();

	switch ( ControlID )
	{
	case LOGIN_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
				
				CString strID = m_pEditBoxMan->GetEditString ( LOGIN_EDIT_ID );
				CString strPW = m_pEditBoxMan->GetEditString ( LOGIN_EDIT_PW );
				CString strRP;
				// 랜덤키 추가%%%
				
				if (sp == SP_HONGKONG || sp == SP_OFFICE_TEST)
				{
//#if defined TW_PARAM || defined HK_PARAM || defined _RELEASED
					strRP = m_pEditBoxMan->GetEditString ( LOGIN_EDIT_RP );
//#endif
				}

				if ( !strID.GetLength () || !strPW.GetLength () )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "LOGINSTAGE_5" ) );
					return ;
				}

				if (sp == SP_CHINA)
				{
//#ifndef CH_PARAM
					if ( !CheckString ( strID ) )
					{
						m_pInterface->DoModal ( ID2GAMEEXTEXT ( "LOGIN_PAGE_ID_ERROR" ) );
						return ;
					}

					if ( !CheckString ( strPW ) )
					{
						m_pInterface->DoModal ( ID2GAMEEXTEXT ( "LOGIN_PAGE_PW_ERROR" ) );
						return ;
					}
				}
//#endif

				//	컨트롤로 부터 아이디와 암호를 가져와 서버에 전송
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				CNetClient* pNetClient = pGlobalStage->GetNetClient();

				int nServerGroup, nServerChannel;
				m_pInterface->GetConnectServerInfo( nServerGroup, nServerChannel );

				//switch ( RENDERPARAM::emSERVICE_TYPE )
				switch(sp)
				{
				//case RENDERPARAM::EMSERVICE_CHINA:
				case SP_CHINA:
					pNetClient->ChinaSndLogin( strID, strPW, strRP, nServerChannel );
					break;

				//case RENDERPARAM::EMSERVICE_JAPAN:
				case SP_JAPAN:
					pNetClient->JapanSndLogin( strID, strPW, nServerChannel );
                    break;
				
				//case RENDERPARAM::EMSERVICE_THAILAND:
				case SP_THAILAND:
					pNetClient->ThaiSndLogin( strID, strPW, nServerChannel );
					break;

				//case RENDERPARAM::EMSERVICE_GLOBAL:
				case SP_GS:
					// GS는 전장서버를 사용하기 때문에 전장서버에 접속여부를 먼저 확인후 로그인한다
					//pNetClient->GsSndLogin( strID, strPW, nServerChannel );
					pNetClient->GsSndWBLogin( strID, strPW, nServerChannel );
					break;

					//
					//mjeon.indonesia
					//
				case SP_INDONESIA:
					pNetClient->IdnSndLogin(strID, strPW, nServerChannel);
					break;

                case SP_WORLD_BATTLE:
                    pNetClient->WorldBattleLogin(strID, strPW, nServerChannel);
                    break;

				case SP_EU:
					pNetClient->GmSndLogin( strID, strPW, nServerChannel );
					break;

				case SP_US:
					pNetClient->UsSndLogin( strID, strPW, nServerChannel );
					break;

				default:
					pNetClient->SndLogin( strID, strPW, strRP, nServerChannel );
					break;
				};

				// 통합전장 서버 접속을 위해서 아이디를 보관
				pGlobalStage->SetWorldBattleID( strID );
				pGlobalStage->SetPWDForWB( strPW );

 
				//	Note : 유저 id 설정. ( 게임 설정 저장, id 저장을 위해서 필요함. )
				//
                if ( RANPARAM::bSAVE_USERID )
                {
				    RANPARAM::SETUSERID( strID );
				    RANPARAM::SAVE(sp);
                }
                else
                {
                    RANPARAM::SETUSERID("");
                    RANPARAM::SAVE(sp);
                }
				
				//	암호 인증 대기 메시지 표시
				m_pInterface->DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_6"), UI::MODAL_INFOMATION, UI::CANCEL, OUTER_MODAL_WAITCONFIRM );
				m_pInterface->SetModalCallWindowID( GetWndID() );

				//m_pInterface->UiHideGroup ( GetWndID () );
				//m_pInterface->UiShowGroupFocus ( SELECT_CHARACTER_PAGE );

                m_pEditBoxMan->EndEdit();
			}
		}
		break;

	case LOGIN_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				CNetClient* pNetClient = pGlobalStage->GetNetClient();
				if( pNetClient->IsOnline() == true )
				{
					pNetClient->CloseConnect();
					m_pInterface->SetCancelToLogin();
				}

				m_pInterface->ToSelectServerPage ( GetWndID () );
			}
		}
		break;

	case LOGIN_PAGE_IDSAVE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( m_pIDSaveButton )
				{
					RANPARAM::bSAVE_USERID = !RANPARAM::bSAVE_USERID;
					RANPARAM::SAVE(m_pGaeaClient->GetServiceProvider());
					LoadIDSaveButton ();
				}
			}
		}
		break;
	}
}

BOOL CLoginPage::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();

	// 문자열 체크 - 들어가면 안되는 특수문자 : ~!@#$%^&*+|":?><\=`',.;[]{}()
	if( STRUTIL::CheckString( strTemp ) )
	{
		m_pInterface->DoModal ( ID2GAMEEXTEXT ( "LOGINSTAGE_4" ), UI::MODAL_INFOMATION, UI::OK );		
		return FALSE;
	}

    return TRUE;
}

void CLoginPage::ResetAll ()
{
	m_pEditBoxMan->EndEdit ();

	m_pEditBoxMan->ClearEdit ( LOGIN_EDIT_ID );
	m_pEditBoxMan->ClearEdit ( LOGIN_EDIT_PW );
	// 랜덤키 추가%%%
//#if defined TW_PARAM || defined HK_PARAM || defined _RELEASED
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	if (sp == SP_OFFICE_TEST || sp == SP_HONGKONG)
	{
		m_pEditBoxMan->ClearEdit ( LOGIN_EDIT_RP );

		m_nRPUpdateCnt = 0;
	}
//#endif

	m_pEditBoxMan->Init();
	m_pEditBoxMan->BeginEdit();

	if( m_pEditBoxMan->IsMODE_NATIVE() )
	{
		m_pEditBoxMan->DoMODE_TOGGLE();
	}

	std::string strSavedUserID = RANPARAM::GETUSERID_DEC();
	if ( !strSavedUserID.empty() && strSavedUserID.size() )
	{
		m_pEditBoxMan->SetEditString ( LOGIN_EDIT_ID, CString ( strSavedUserID.c_str() ) );
		m_pEditBoxMan->GoNextTab ();
	}
}

void CLoginPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );
    DXInputString* pInputString = m_pEngineDevice->GetInputString();
	if (bVisible)
	{
		if (GLCONST_CHAR::nUI_KEYBOARD == 2)
		{            
			m_pEditBoxMan->DisableKeyInput();
			pInputString->DisableKeyInput();
		}

		ResetAll();

		if( m_pRandTextBox ) m_pRandTextBox->ClearText();
		m_pInterface->ResetCancelToLogin();
	}
	else
	{
		m_pEditBoxMan->EndEdit ();

		if( GLCONST_CHAR::nUI_KEYBOARD == 2 )
		{
			m_pEditBoxMan->UsableKeyInput();
			pInputString->UsableKeyInput();
		}

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
		CNetClient* pNetClient = pGlobalStage->GetNetClient();
		pNetClient->ResetRandomPassNumber();
	}
}

CBasicButton* CLoginPage::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );

	return pButton;
}

void CLoginPage::LoadIDSaveButton ()
{
	BOOL bIDSave = RANPARAM::bSAVE_USERID;
	m_pIDSaveButton->SetFlip ( bIDSave );	
}

void CLoginPage::SetCharToEditBox( TCHAR cKey )
{
	if( !m_pEditBoxMan ) return;
	
	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	strTemp += cKey;

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CLoginPage::DelCharToEditBox()
{
	if( !m_pEditBoxMan ) return;

	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	INT nLenth = strTemp.GetLength();
	strTemp = strTemp.Left( nLenth - 1 );

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CLoginPage::GoNextTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoNextTab();
}

void CLoginPage::GoThisTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoThisTab();
}
