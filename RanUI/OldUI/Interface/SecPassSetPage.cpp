#include "StdAfx.h"
#include "../../../SigmaCore/DebugSet.h"
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

#include "../../../RanLogicClient/Network/s_NetClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../OuterInterface.h"
#include "SecPassSetPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int	CSecPassSetPage::nLIMIT_ID = 16;
const int	CSecPassSetPage::nLIMIT_PW = 16;

#define IsNumber(ch)      ((ch) >= '0' && (ch) <= '9')


CSecPassSetPage::CSecPassSetPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIOuterWindow(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
{
}

CSecPassSetPage::~CSecPassSetPage ()
{
}

void CSecPassSetPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nBUTTONSIZE = CBasicTextButton::SIZE19;

	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "SECPASS_SETPAGE_ID", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_CHECKPAGE_IDPW", 0 ) );
	pTextBox = CreateStaticControl ( "SECPASS_SETPAGE_PW", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_PW", 0 ) );
	pTextBox = CreateStaticControl ( "SECPASS_SETPAGE_PWCHECK", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_PW", 1 ) );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_SETPAGE_ID_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_SETPAGE_PW_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "SECPASS_SETPAGE_PWCHECK_BACK" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
		pEditBoxMan->CreateSub ( this, "SETSECPASS_EDIT_MAN", UI_FLAG_DEFAULT, SETSECPASS_EDIT_MAN );
		pEditBoxMan->CreateEditBox ( SECPASS_SETPAGE_EDIT_ID, "SECPASS_SETPAGE_EDIT_ID", "SECPASS_SETPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_ID );
		pEditBoxMan->CreateEditBox ( SECPASS_SETPAGE_EDIT_PW, "SECPASS_SETPAGE_EDIT_PW", "SECPASS_SETPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW );
		pEditBoxMan->CreateEditBox ( SECPASS_SETPAGE_EDIT_PWCHECK, "SECPASS_SETPAGE_EDIT_PWCHECK", "SECPASS_SETPAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW );		
		pEditBoxMan->SetHide ( SECPASS_SETPAGE_EDIT_PW, TRUE );
		pEditBoxMan->SetHide ( SECPASS_SETPAGE_EDIT_PWCHECK, TRUE );
		RegisterControl( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;
	}

	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, SETSECPASS_OK );
	pOKButton->CreateBaseButton ( "SECPASS_SETPAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_OKCANCEL", 0 ) );
	pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( pOKButton );

	CBasicTextButton* pQuitButton = new CBasicTextButton(m_pEngineDevice);
	pQuitButton->CreateSub ( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, SETSECPASS_CANCEL );
	pQuitButton->CreateBaseButton ( "SECPASS_SETPAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "SECPASS_SETPAGE_OKCANCEL", 1 ) );
	pQuitButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pQuitButton );

}	

void CSecPassSetPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
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
	//	탭 이동
	if ( m_pEditBoxMan )
	{
		if ( m_pEditBoxMan->IsMODE_NATIVE() ) m_pEditBoxMan->DoMODE_TOGGLE(); 


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

void CSecPassSetPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SETSECPASS_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
				
				CString strID = m_pEditBoxMan->GetEditString ( SECPASS_SETPAGE_EDIT_ID );
				CString strPW = m_pEditBoxMan->GetEditString ( SECPASS_SETPAGE_EDIT_PW );
				CString strPWCHECK = m_pEditBoxMan->GetEditString ( SECPASS_SETPAGE_EDIT_PWCHECK );
				
				if (  !strID.GetLength () || !strPW.GetLength () || !strPWCHECK.GetLength () )  // 비번을 입력해주세요.
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "LOGINSTAGE_5" ) );
					ResetAll();
					return ;
				}

				if ( strID.GetLength() < 4 || strID.GetLength() > 16 )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "SECID_LENGTH_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( strPW.GetLength() < 8 || strPW.GetLength() > 16 )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "SECPASS_LENGTH_ERROR" ) );
					ResetAll();
					return ;
				}

				if( strPW != strPWCHECK ) // 비번 설정 불일치
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "SECPASS_NOTEQUAL_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( !CheckString ( strID ) )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "LOGIN_PAGE_ID_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( !CheckString ( strPW ) )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "LOGIN_PAGE_PW_ERROR" ) );
					ResetAll();
					return ;
				}

				if ( strID == strPW ) 
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "SECPASS_EQUAL_ERROR" ) );
					ResetAll();
					return ;
				}

				// ID PW 첫문자
				// 영문만 입력

				if ( IsNumber( strID.GetAt(0) ) || IsNumber( strPW.GetAt(0) ) )
				{
					m_pInterface->DoModal ( ID2GAMEEXTEXT ( "SECPASS_FIRST_NUMBER" ) );
					ResetAll();
					return ;
				}

				//	컨트롤로 부터 아이디와 암호를 가져와 서버에 전송
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				CNetClient* pNetClient = pGlobalStage->GetNetClient();

				int nServerGroup, nServerChannel;
				m_pInterface->GetConnectServerInfo( nServerGroup, nServerChannel );

                EMSERVICE_PROVIDER ServiceProvider = m_pEngineDevice->GetServiceProvider();
//#if defined(MY_PARAM) || defined(MYE_PARAM)
                if (ServiceProvider == SP_MALAYSIA || ServiceProvider == SP_MALAYSIA_EN)
                {
				    pNetClient->TerraSndPassCheck( m_pInterface->GetDaumGID(), strPW, 0 );
                }
//#elif defined(KRT_PARAM) || defined(KR_PARAM)
                else if (ServiceProvider == SP_KOREA || ServiceProvider == SP_KOREA_TEST)
                {
				    pNetClient->DaumSndPassCheck( m_pInterface->GetDaumGID(), strPW, 0 );
                }
//#elif defined(JP_PARAM)	// 2차 패스워드
                else if (ServiceProvider == SP_JAPAN)
                {
				    pNetClient->ExciteSndPassCheck( strID, strPW, 0 );
                }
//#endif
                else
                {
                }

				m_pInterface->DoModal(ID2GAMEEXTEXT("LOGINSTAGE_6"), UI::MODAL_INFOMATION, UI::CANCEL, OUTER_MODAL_WAITCONFIRM);
				m_pInterface->SetModalCallWindowID(GetWndID());
			}
		}
		break;

	case SETSECPASS_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				CNetClient* pNetClient = pGlobalStage->GetNetClient();
				if (pNetClient->IsOnline() == true)
				{
					pNetClient->CloseConnect();
					m_pInterface->SetCancelToLogin();
				}

				m_pInterface->ToSelectServerPage ( GetWndID () );
			}
		}
		break;
	}
}

BOOL CSecPassSetPage::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();

	// 문자열 체크 - 들어가면 안되는 특수문자 : ~!@#$%^&*+|":?><\=`',.;[]{}()
	if( STRUTIL::CheckString( strTemp ) )
	{
		return FALSE;
	}

    return TRUE;
}

void CSecPassSetPage::ResetAll ()
{
	m_pEditBoxMan->EndEdit ();

	m_pEditBoxMan->ClearEdit ( SECPASS_SETPAGE_EDIT_ID );
	m_pEditBoxMan->ClearEdit ( SECPASS_SETPAGE_EDIT_PW );
	m_pEditBoxMan->ClearEdit ( SECPASS_SETPAGE_EDIT_PWCHECK );

	m_pEditBoxMan->Init();
	m_pEditBoxMan->BeginEdit();

	if( m_pEditBoxMan->IsMODE_NATIVE() )
	{
		m_pEditBoxMan->DoMODE_TOGGLE();
	}
}

void CSecPassSetPage::SetVisibleSingle ( BOOL bVisible )
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

		m_pInterface->OpenMessageWindow( GetWndID(), bVisible );
		m_pInterface->ResetCancelToLogin();
	}
	else
	{
		m_pEditBoxMan->EndEdit();
		if (GLCONST_CHAR::nUI_KEYBOARD == 2)
		{
			m_pEditBoxMan->UsableKeyInput();
			pInputString->UsableKeyInput();
		}		
		m_pInterface->OpenMessageWindow(GetWndID(), bVisible);
	}
}

void CSecPassSetPage::GoNextTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoNextTab();
}

void CSecPassSetPage::GoThisTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoThisTab();
}

void CSecPassSetPage::SetCharToEditBox( TCHAR cKey )
{
	if( !m_pEditBoxMan ) return;
	
	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	strTemp += cKey;

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CSecPassSetPage::DelCharToEditBox()
{
	if( !m_pEditBoxMan ) return;

	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	INT nLenth = strTemp.GetLength();
	strTemp = strTemp.Left( nLenth - 1 );

	m_pEditBoxMan->SetEditString( nID, strTemp );
}