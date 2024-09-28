#include "StdAfx.h"

#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"

#include "../RanLogic/RANPARAM.h"

#include "OldUI/Interface/SelectServerPage.h"
#include "OldUI/Interface/LoginPage.h"
#include "OldUI/Interface/SelectCharacterPage.h"
#include "OldUI/Interface/CreateCharacterPage.h"
#include "OldUI/Interface/WaitDialogue.h"
#include "OldUI/Interface/SelectCharacterLeftPage.h"
#include "OldUI/Interface/MessageWindow.h"
#include "OldUI/Interface/NewCreateCharPage.h"

#include "OldUI/Util/ModalWindowOuter.h"
#include "./StaticUIManager.h"
#include "OuterInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void COuterInterface::WAITSERVER_DIALOGUE_OPEN ( const CString& strMessage, const int nAction, const float fTimer )
{
	if ( UiIsVisibleGroup ( WAIT_DISPLAY ) ) return ;

	m_pWaitDisplay->SetPrevPageID ( GetRenderStage () );

	UiShowGroupFocus ( WAIT_DISPLAY );	
	m_pWaitDisplay->SetDisplayMessage ( strMessage, nAction, fTimer );
}

void COuterInterface::WAITSERVER_DIALOGUE_CLOSE ()
{
	UiHideGroup ( WAIT_DISPLAY );
}

UIGUID COuterInterface::GetRenderStage ()
{
	CUIControl* pFocusControl = uiman::GetOuterUIMan().GetFocusControl();
	if ( pFocusControl ) 
	{
		UIGUID cID = pFocusControl->GetWndID ();
		switch ( cID )
		{
		case MODAL_WINDOW_OUTER:	return m_pModalWindow->GetPrevPageID ();
		case WAIT_DISPLAY:			return m_pWaitDisplay->GetPrevPageID ();
		default:					return cID;
		}		
	}
	return NO_ID;
}

void COuterInterface::CloseAllWindow ()
{	
	CUIFocusContainer::UICONTROL_FOCUSLIST list = uiman::GetOuterUIMan().GetFocusList()->GetFocusList ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_CITER citer = list.begin ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		UiHideGroup ( citer->cID );
	}
}

void COuterInterface::ToChinaAreaPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	if( cID != SELECT_CHINA_AREA_PAGE )
	{
		UiHideGroup ( cID, bMakeMsg );
		UiShowGroupFocus ( SELECT_CHINA_AREA_PAGE, bMakeMsg );
	}
}

void COuterInterface::ToSelectServerPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	if( LOGIN_PAGE == cID || SECPASS_SETPAGE == cID || SECPASS_CHECKPAGE == cID )
	{
		UiHideGroup( PASS_KEYBOARD, bMakeMsg );
	}

	UiHideGroup ( cID, bMakeMsg );
	UiHideGroup ( TITLE_TEXT, bMakeMsg );
	UiShowGroupBottom ( COPYRIGHT, bMakeMsg );
	UiShowGroupBottom ( LOGIN_MARK );

	if (m_pGaeaClient->GetServiceProvider() == SP_CHINA)
	{
//#ifdef CH_PARAM
		UiShowGroupBottom ( CHINA_TITLE, bMakeMsg );
	}
//#endif

	if ( m_pSelectServerPage )
	{	
		m_pSelectServerPage->ResetAll ();		
		UiShowGroupFocus ( SELECT_SERVER_PAGE, bMakeMsg );
	}
}

void	COuterInterface::ToLoginPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	UiHideGroup ( cID, bMakeMsg );
	UiHideGroup ( TITLE_TEXT, bMakeMsg );
	UiShowGroupBottom ( COPYRIGHT, bMakeMsg );
	UiShowGroupBottom ( LOGIN_MARK );

//#ifdef CH_PARAM
	if (m_pGaeaClient->GetServiceProvider() == SP_CHINA)
	{
		UiShowGroupBottom ( CHINA_TITLE, bMakeMsg );
	}
//#endif

	if ( m_pLoginPage )
	{
		if( GLCONST_CHAR::nUI_KEYBOARD != 1 )
		{
			UiShowGroupFocus( PASS_KEYBOARD, bMakeMsg );
		}

		UiShowGroupFocus( LOGIN_PAGE, bMakeMsg );
	}
}

void COuterInterface::ToSecPassPage( UIGUID cID, int nCheckFlag )
{
    bool bMakeMsg = true;

	UiHideGroup ( cID, bMakeMsg );
	UiHideGroup ( TITLE_TEXT, bMakeMsg );
	UiShowGroupBottom ( COPYRIGHT, bMakeMsg );

	if( nCheckFlag == 0 ) // 비번 설정
	{
		if ( m_pSecPassSetPage )  
		{
			if( GLCONST_CHAR::nUI_KEYBOARD != 1 )
			{
				UiShowGroupFocus( PASS_KEYBOARD, bMakeMsg );
			}

			UiShowGroupFocus( SECPASS_SETPAGE, bMakeMsg );			
		}
	}
	else if( nCheckFlag == 1 )
	{
		if ( m_pSecPassCheckPage )  
		{
			if( GLCONST_CHAR::nUI_KEYBOARD != 1 )
			{
				UiShowGroupFocus( PASS_KEYBOARD, bMakeMsg );
			}

			UiShowGroupFocus( SECPASS_CHECKPAGE, bMakeMsg );
		}

	}
}

void COuterInterface::ToSelectCharacterPage( UIGUID cID )
{
	bool bMakeMsg = true;

	if( LOGIN_PAGE == cID || SECPASS_SETPAGE == cID || SECPASS_CHECKPAGE == cID )
	{
		UiHideGroup( SELECT_SERVER_PAGE, bMakeMsg ); // MEMO : 대만일 경우 서버 페이지를 닫아준다.
		UiHideGroup( PASS_KEYBOARD, bMakeMsg );
	}
	else if( CREATE_CHARACTER_PAGE == cID )
	{
		UiHideGroup( CREATE_CHARACTER_WEB, bMakeMsg );
		UiHideGroup( CREATE_CHARACTER_BUTTON_PAGE, bMakeMsg );
		UiHideGroup( ROTATE_CHARACTER, bMakeMsg );
	}

	UiHideGroup ( cID, bMakeMsg );
	UiHideGroup ( COPYRIGHT, bMakeMsg );

//#ifdef CH_PARAM
	if (m_pGaeaClient->GetServiceProvider() == SP_CHINA)
	{
		UiHideGroup ( CHINA_TITLE, bMakeMsg );
	}
//#endif

	if ( m_pSelectServerPage )
	{
		m_pSelectCharacterPage->ResetAll();

		if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
		{
			UiShowGroupFocus( SELECT_COUNTRY_UI, bMakeMsg );
		}

		UiShowGroupFocus( SELECT_CHARACTER_LEFTPAGE, bMakeMsg );
		UiShowGroupFocus( SELECT_CHARACTER_PAGE, bMakeMsg );

		CString strText;
		strText.Format( _T( "< %s >" ), ID2GAMEWORD( "ESC_MENU" ) );
		m_pTitleText->SetText( strText );
		UiShowGroupBottom ( TITLE_TEXT, bMakeMsg );
	}

	bMakeMsg = true;

	if( SECPASS_SETPAGE == cID || SECPASS_CHECKPAGE == cID )
	{
		UiHideGroup( SELECT_SERVER_PAGE, bMakeMsg ); // MEMO : 대만일 경우 서버 페이지를 닫아준다.
		UiHideGroup( PASS_KEYBOARD, bMakeMsg );
	}
	else if( CREATE_CHARACTER_PAGE == cID )
	{
		UiHideGroup( CREATE_CHARACTER_WEB, bMakeMsg );
		UiHideGroup( CREATE_CHARACTER_BUTTON_PAGE, bMakeMsg );
		UiHideGroup( ROTATE_CHARACTER, bMakeMsg );
	}

	if ( m_pSelectContinentListUI )
		m_pSelectContinentListUI->UpdateList();
}

void	COuterInterface::ToCreateCharacterPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	if( LOGIN_PAGE == cID || SECPASS_SETPAGE == cID || SECPASS_CHECKPAGE == cID )
	{
		UiHideGroup( PASS_KEYBOARD, bMakeMsg );
	}

	UiHideGroup( cID, bMakeMsg );

	if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
	{
		UiHideGroup( SELECT_COUNTRY_UI, bMakeMsg );
		UiHideGroup( SELECT_CONTINENT_LIST_UI, bMakeMsg );
		UiHideGroup( SELECT_COUNTRY_LIST_UI, bMakeMsg );
	}
	UiHideGroup( SELECT_CHARACTER_PAGE, bMakeMsg );
	UiHideGroup( SELECT_CHARACTER_LEFTPAGE, bMakeMsg );
	UiHideGroup( COPYRIGHT, bMakeMsg );
    UiHideGroup( CHARACTER_COUNTRY_CLUB_INFO, bMakeMsg );

//#ifdef CH_PARAM
	if (m_pGaeaClient->GetServiceProvider() == SP_CHINA)
	{
		UiHideGroup ( CHINA_TITLE, bMakeMsg );
	}
//#endif

	if ( m_pSelectServerPage )
	{
		UiShowGroupFocus ( CREATE_CHARACTER_PAGE, bMakeMsg );
		UiShowGroupFocus ( CREATE_CHARACTER_BUTTON_PAGE, bMakeMsg );
		UiShowGroupTop( ROTATE_CHARACTER, bMakeMsg );
		CString strText;
		strText.Format( _T( "< %s >" ), ID2GAMEWORD( "CREATE_CHARACTER_PAGE" ) );
		m_pTitleText->SetText( strText );
		UiShowGroupBottom ( TITLE_TEXT, bMakeMsg );
	}
}

void COuterInterface::ToSelectCountryPage()
{
    if ( UiIsVisibleGroup( LOGIN_PAGE ) )
        UiHideGroup( LOGIN_PAGE );

    if ( UiIsVisibleGroup( PASS_KEYBOARD ) )
        UiHideGroup( PASS_KEYBOARD );

    if ( m_pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
    {
        if ( m_pSelectCountryConfirmModal )
        {
            ShowBeforeSelectModal();
        }
    }
}

void COuterInterface::ShowSelectCountryWindow()
{
    if ( UiIsVisibleGroup( SELECT_COUNTRY_CONFIRM_MODAL ) )
        UiHideGroup( SELECT_COUNTRY_CONFIRM_MODAL );

    UiShowGroupFocus( SELECT_COUNTRY_WINDOW );
}

void COuterInterface::ShowSelectClubWindow()
{
    if ( UiIsVisibleGroup( SELECT_COUNTRY_CONFIRM_MODAL ) )
        UiHideGroup( SELECT_COUNTRY_CONFIRM_MODAL );

    m_pSelectClubWindow->SetCountryInfo( GetCountry() );
    UiShowGroupFocus( SELECT_CLUB_WINDOW );
}

void COuterInterface::SendGameJoinMessage()
{
    if ( m_pSelectCharacterPage )
        m_pSelectCharacterPage->SndGameJoinMsg();
}

const char* COuterInterface::MakeString ( const char* szFormat, ... )
{
	const int MAX_STRLENGTH = 256;

	va_list argList;
	static	char szBuffer[MAX_STRLENGTH];

	va_start(argList, szFormat);
	StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
	va_end(argList);

	return szBuffer;
}

BOOL	COuterInterface::LoadClientVersion ( int& nPatchVer, int& nGameVer )
{
	//	NOTE
	//		런처에도 버전을 로드하는 부분이 있습니다.
	//		변경될 경우 반드시 체크 해야합니다.
	//		AutoPatchManDlgEx.cpp!!!

	const CString strVersionFileName = "cVer.bin";

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CString strTemp;
	strTemp.Format ( "%s\\%s", pGlobalStage->GetAppPath(), strVersionFileName );
	FILE* cfp = _fsopen ( strTemp.GetString(), "rb", _SH_DENYNO );
	if ( !cfp )
	{
		return FALSE;
	}
	if ( 1 != fread ( &nPatchVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );
		return FALSE;
	}
	if ( 1 != fread ( &nGameVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );		
		return FALSE;
	}
	fclose ( cfp );

	return TRUE;
}

void	COuterInterface::START_WAIT_TIME ( float fTime )
{
	m_bUseWaitTime = TRUE;
	m_fWaitTime = fTime;
}

void	COuterInterface::UPDATE_WAIT_TIME ( float fElapsedTime )
{
	if ( m_bUseWaitTime )
	{
		m_fWaitTime -= fElapsedTime;

		if ( m_fWaitTime < 0.0f ) m_bUseWaitTime = FALSE;
	}
}

HRESULT COuterInterface::FinalCleanup ()
{
	HRESULT hr;

    hr = uiman::GetOuterUIMan().FinalCleanup ();
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void	COuterInterface::DeleteCharacter ()
{
	//	캐릭터 삭제 요청.
	//	Step 1.	2차 비밀번호 서버 전송
	//
	// 컨트롤로 부터 아이디와 암호를 가져와 서버에 전송
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	SCHARINFO_LOBBY* pCharInfo = m_pSelectCharacterLeftPage->GetDelCharInfo ();

	if(!pCharInfo)
		return;

	switch(RENDERPARAM::emSERVICE_TYPE)
	{
	case RENDERPARAM::EMSERVICE_KOREA:
		{
			CString strKor = GetModalWindow()->GetEditString();
			pNetClient->DaumSndDelCharacter(pCharInfo->m_dwCharID, strKor.GetString());			
		}
		break;

	case RENDERPARAM::EMSERVICE_INDONESIA:
		{
			//
			//mjeon.indonesia.2차 패스워드 갖고 오기
			//
			CString strIDN = GetModalWindow()->GetEditString();
			pNetClient->IdnSndDelCharacter(pCharInfo->m_dwCharID, strIDN.GetString());
		}
		break;

	case RENDERPARAM::EMSERVICE_JAPAN:
		pNetClient->JapanSndDelCharacter(pCharInfo->m_dwCharID);
		break;

	//case EMSERVICE_GSP:
	//	pNetClient->GspSndDelCharacter ( pCharInfo->m_dwCharID );
	//	break;

	case RENDERPARAM::EMSERVICE_MALAYSIA_CN:
	case RENDERPARAM::EMSERVICE_MALAYSIA_EN:
	case RENDERPARAM::EMSERVICE_PHILIPPINES:
	case RENDERPARAM::EMSERVICE_VIETNAM:
		pNetClient->TerraSndDelCharacter ( pCharInfo->m_dwCharID );
		break;

	case RENDERPARAM::EMSERVICE_CHINA:
		{
			CString strChina = GetModalWindow()->GetEditString ();
			pNetClient->ChinaSndDelCharacter ( pCharInfo->m_dwCharID, strChina.GetString () );
		}
		break;

	case RENDERPARAM::EMSERVICE_GLOBAL:
		{
			CString strGS = GetModalWindow()->GetEditString();
			pNetClient->GsSndDelCharacter ( pCharInfo->m_dwCharID, strGS.GetString() );
		}
		break;
//	case EMSERVICE_INDONESIA:
//		{
//			pNetClient->IdSndDelCharacter ( pCharInfo->m_dwCharID );
//		}
//		break;
	case RENDERPARAM::EMSERVICE_EU:
		{
			pNetClient->GmSndDelCharacter ( pCharInfo->m_dwCharID );
		}
		break;
	case RENDERPARAM::EMSERVICE_US:
		{
			pNetClient->UsSndDelCharacter ( pCharInfo->m_dwCharID );
		}
		break;
	default: // 대만 태국
		{
			CString strTemp = GetModalWindow()->GetEditString ();
			pNetClient->SndDelCharacter ( pCharInfo->m_dwCharID, strTemp.GetString () );
		}
		break;
	};
}

void	COuterInterface::SetConnectServerInfo ( const int nServerGroup, const int nServerChannel )
{
	m_nServerGroup = nServerGroup;
	m_nServerChannel = nServerChannel;
}

void	COuterInterface::GetConnectServerInfo ( int& nServerGroup, int& nServerChannel )
{
	nServerGroup = m_nServerGroup;
	nServerChannel = m_nServerChannel;
}

void COuterInterface::ISetResolution( long lResolution )
{
    CUIMan::SetResolution( lResolution );
}

void COuterInterface::OpenMessageWindow ( UIGUID cID, BOOL bOpen )
{
	if ( !m_pMessageWindow ) return;

	if ( bOpen )
	{
		if ( cID == SECPASS_SETPAGE )
		{
			m_pMessageWindow->SetMessage( ID2GAMEEXTEXT ( "SECPASS_SETPAGE_TEXT") );
		}
		else if ( cID == SECPASS_CHECKPAGE )
		{
			m_pMessageWindow->SetMessage( ID2GAMEEXTEXT ( "SECPASS_CHECKPAGE_TEXT") );
		}

		UiShowGroupTop( MESSAGE_WINDOW );
	}
	else
	{
		UiHideGroup( MESSAGE_WINDOW );
	}
}

const UIRECT COuterInterface::GetBackImageRect()	
{ 
	return m_pNewCreateCharPage->m_pBackImage->GetGlobalPos();
}

void COuterInterface::SHOW_COMMON_LINEINFO ( const CString& strText, D3DCOLOR dwColor )
{
	if ( uiman::GetOuterUIMan().IsMouseInControl () ) return ;
	if ( UiIsVisibleGroup ( OUTER_INFO_DISPLAY ) ) return ;		

	if ( !strText.GetLength() ) return ;

	if ( m_strGeneralInfoBack != strText )
	{
		m_pInfoDisplay->ClearText ();
		m_pInfoDisplay->AddTextNoSplit ( strText, dwColor );

		m_strGeneralInfoBack = strText;
	}

	UiShowGroupTop ( OUTER_INFO_DISPLAY );

	//m_bUSING_INFO_DISPLAY = true;
}

bool COuterInterface::BEGIN_COMMON_LINEINFO_MULTI ( int nAlignFlag )
{
	if ( uiman::GetOuterUIMan().IsMouseInControl () ) return false;
	if ( UiIsVisibleGroup ( OUTER_INFO_DISPLAY ) ) return false;

	m_strGeneralInfoBack.Empty ();
	m_pInfoDisplay->ClearText ();
	m_pInfoDisplay->SetTextAlign( nAlignFlag );

	return true;
}

bool COuterInterface::ADD_COMMON_LINEINFO_MULTI ( const CString& strText, D3DCOLOR dwColor )
{
	if ( UiIsVisibleGroup ( OUTER_INFO_DISPLAY ) ) return false;

	m_pInfoDisplay->AddTextNoSplit ( strText, dwColor );

	return true;
}

void COuterInterface::END_COMMON_LINEINFO_MULTI ()
{
	UiShowGroupTop ( OUTER_INFO_DISPLAY );
}

void COuterInterface::CLOSE_MODAL ( const UIGUID cID, bool bMakeMsg )
{		
	CUIControl* pControl = uiman::GetOuterUIMan().GetFocusControl();	
	if ( pControl && pControl->GetWndID () != cID )
	{
		if ( !((CModalWindow*)pControl)->IsUseModaless () )
		{
			GASSERT ( 0 && "오류, 모달 삭제가 정상적이지 않습니다." );
		}
	}
	UiHideGroup ( cID, bMakeMsg );
}
