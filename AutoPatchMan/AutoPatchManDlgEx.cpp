#include "stdafx.h"
#include <afxinet.h>
#include "../SigmaCore/DebugSet.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicClient/Network/s_CPatch.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"
#include "GlobalVariable.h"
#include "LogControl.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//BOOL	CAutoPatchManDlg::ConnectFtpServer ( CString strFtpAddress )
//{
//	int nRetCode;	
//
//	Sleep( 500 );
//	return FALSE;
//	
//    
//#ifdef	__ALPHA__
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						g_szAlphaFtpID,
//						g_szAlphaFtpPW,
//						RANPARAM::bUsePassiveDN );
//#else
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						NS_GLOBAL_VAR::g_szBetaFtpID,
//						NS_GLOBAL_VAR::g_szBetaFtpPW,
//						RANPARAM::bUsePassiveDN );
//#endif
//    	
//	if (nRetCode == NET_ERROR)
//	{
//		return FALSE;
//	}
//	return TRUE;
//}
//
//BOOL	CAutoPatchManDlg::DisconnectFtpServer ()
//{
//	if ( m_pFtpPatch )
//	{
//		m_pFtpPatch->DisConnect();		
//	}
//
//	return TRUE;
//}

void	CAutoPatchManDlg::InitDlgCtrl ()
{
	m_ctrlDownLoad.SubclassDlgItem( IDC_STATIC_DOWNLOAD, this );
	m_ctrlDownLoad2.SubclassDlgItem( IDC_STATIC_DOWNLOAD2, this );

	m_ctrlStart.SetBitmaps(IDB_BITMAP_START_ON, IDB_BITMAP_START_NOR, IDB_BITMAP_START_DN, IDB_BITMAP_START_DIS );
	m_ctrlStart.SizeToContent();

	m_ctrlUpdate.SetBitmaps(IDB_BITMAP_UPDATE_ON, IDB_BITMAP_UPDATE_NOR, IDB_BITMAP_UPDATE_DN, IDB_BITMAP_UPDATE_DIS );
	m_ctrlUpdate.SizeToContent();

	m_ctrlRetry.SetBitmaps(IDB_BITMAP_RETRY_ON, IDB_BITMAP_RETRY_NOR, IDB_BITMAP_RETRY_DN, IDB_BITMAP_RETRY_DIS );
	m_ctrlRetry.SizeToContent();

	m_ctrlOption.SetBitmaps( IDB_BITMAP_OPTION_ON, IDB_BITMAP_OPTION_NOR, IDB_BITMAP_OPTION_DN, IDB_BITMAP_OPTION_DIS );
	m_ctrlOption.SizeToContent();

	m_ctrlItem.SetBitmaps( IDB_BITMAP_ITEM_ON, IDB_BITMAP_ITEM_NOR, IDB_BITMAP_ITEM_DN, IDB_BITMAP_ITEM_NOR );
	m_ctrlItem.SizeToContent();

	m_ctrlManualPatch.SetBitmaps( IDB_BITMAP_manualpatch_ON, IDB_BITMAP_MANUALPATCH_NOR, IDB_BITMAP_MANUALPATCH_DN, IDB_BITMAP_MANUALPATCH_NOR );
	m_ctrlManualPatch.SizeToContent();	

	m_ctrlExit.SetBitmaps( IDB_BITMAP_EXIT_ON, IDB_BITMAP_EXIT_NOR, IDB_BITMAP_EXIT_DN, IDB_BITMAP_EXIT_NOR );
	m_ctrlExit.SizeToContent();	

	m_ctrlCheckBox.SetIcon(IDI_CHECK_ON, IDI_CHECK_OFF);
	m_ctrlCheckBox.DrawTransparent();

	CBitmap nBitmap;
	
	nBitmap.LoadBitmap(IDB_BITMAP_PROGRESS);

	CWnd* poProgress = GetDlgItem( IDC_PROGRESS_CUR );
    m_ctrlProgressCur = new CSkinProgress( poProgress, 100, &nBitmap, false );

	poProgress = GetDlgItem( IDC_PROGRESS_ALL );
    m_ctrlProgressAll = new CSkinProgress( poProgress, 100, &nBitmap, false );


	m_ctrlListBox.SetBitmapID(IDB_BITMAP_LIST_BACK);
	m_ctrlListBox.Init();
	m_ctrlListBox.SetColor( RGB(192,192,192),RGB(255,0,0) );

	m_ctrlHomePage.SetColours( RGB(255,128,64),m_ctrlHomePage.GetVisitedColour() );
	m_ctrlHomePage.SetURL( RANPARAM::HomePageAddress );
	m_ctrlHomePage.SetUnderline(CHyperLink::ulAlways);

// 메인 로고 변경
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	
//#if defined( TW_PARAM ) || defined ( HK_PARAM ) || defined ( JP_PARAM )
	if (sp == SP_GS || sp == SP_HONGKONG || sp == SP_JAPAN)
	{	
		int nBitMap = IDB_BITMAP_BACK_LOGO;

		/*
#if defined ( TW_PARAM ) || defined ( HK_PARAM )
		nBitMap = IDB_BITMAP_BACK_LOGO_TW;
#elif defined ( JP_PARAM )  
		nBitMap = IDB_BITMAP_BACK_LOGO_JP;
#endif
		*/

		if (sp == SP_JAPAN)
		{
			nBitMap = IDB_BITMAP_BACK_LOGO_JP;
		}
		else
		{
			nBitMap = IDB_BITMAP_BACK_LOGO_TW;
		}

		CStatic* pStatic = (CStatic*)GetDlgItem( IDC_STATIC_BACK_LOGO );

		if ( pStatic )
		{
			HINSTANCE hInst = ::AfxGetInstanceHandle();
			HBITMAP hBitmap = ::LoadBitmap(hInst, MAKEINTRESOURCE(nBitMap));
			if ( hBitmap ) 
			{
				pStatic->SetBitmap( hBitmap );
			}
		}
	}
//#endif 
	m_comboNation.SetComboBitmap(IDB_COMBO_LEFT,IDB_COMBO_RIGHT,IDB_COMBO_CEN);
	m_comboNation.SetComboListBitmap(IDB_LIST_LEFT, IDB_LIST_LEFT,IDB_LIST_TOP,IDB_LIST_TOP);
	m_stNATION.SubclassDlgItem( IDC_MNATION_STATIC, this );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CWnd* Wnd = GetDlgItem(IDC_MNATION_STATIC);
		Wnd->ShowWindow(SW_SHOW);
		m_comboNation.ShowWindow(SW_SHOW);
		m_comboNation.SetHighlightTextColor(RGB(255,255,0));
		m_comboNation.SetNormalPositionTextColor(RGB(255,255,0));
	}
}

void	CAutoPatchManDlg::InitDlgText()
{
	SetDlgItemText(IDC_TITLE_STATIC,ID2LAUNCHERTEXT("IDC_TITLE_STATIC"));
	SetDlgItemText(IDC_STATIC_DOWNLOAD, ID2LAUNCHERTEXT("IDC_STATIC_DOWNLOAD"));
	SetDlgItemText(IDC_STATIC_DOWNLOAD2, ID2LAUNCHERTEXT("IDC_STATIC_DOWNLOAD2"));
	m_ctrlDownLoad.Invalidate();
	m_ctrlDownLoad2.Invalidate();
	SetDlgItemText(IDC_STATIC_HOMEPAGE, ID2LAUNCHERTEXT("IDC_STATIC_HOMEPAGE"));
	SetDlgItemText(IDC_BUTTON_START,"");
	SetDlgItemText(IDC_BUTTON_UPDATE,"");
	SetDlgItemText(IDC_BUTTON_RETRY,"");
	SetDlgItemText(IDC_BUTTON_OPTION,"");
	SetDlgItemText(IDC_BUTTON_EXIT,"");
	SetDlgItemText(IDC_READYTOSTART,ID2LAUNCHERTEXT("IDC_READYTOSTART"));
	SetDlgItemText(IDC_BUTTON_PATCH,ID2LAUNCHERTEXT("IDC_BUTTON_PATCH"));
	SetDlgItemText(IDC_CHECK_USE_HTTP,ID2LAUNCHERTEXT("IDC_CHECK_USE_HTTP"));
	m_ctrlHomePage.SetAutoSize( true );

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		const CString strTextName = CLauncherTextMan::GetInstance().GetText("PROVIDE_TEXT_NAME", 0);
		std::vector<std::string> vecText;
		if ( CLauncherTextMan::GetInstance().LoadHelpCommentString(strTextName.GetString(), vecText) )
		{
			size_t i, nSize = vecText.size();
			std::string strArray[10];
			for ( i = 0; i < nSize; ++i )
				strArray[i] = vecText[i];

			SetWin_Combo_Init ( this, IDC_MNATION_COMBO, strArray, nSize );

			std::vector<std::string> vecFlag;
			const CString strFlagName = CLauncherTextMan::GetInstance().GetText("PROVIDE_FLAG_NAME", 0);
			if ( CLauncherTextMan::GetInstance().LoadHelpCommentString(strFlagName.GetString(),vecFlag) )
			{
				size_t i, nSize = vecFlag.size();
				for ( i = 0; i < nSize; ++i )
				{
					if ( strcmp(vecFlag[i].c_str(), RANPARAM::strCountry.GetString()) == 0 )
					{
						SetWin_Text(this, IDC_MNATION_COMBO, vecText[i].c_str());
						break;
					}
				}
			}
		}
		else
		{
			CString strTemp("[GAME TEXT ERROR]Not exist PROVIDE_TEXT_NAME!");
			AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
		}

		const CString strFlag = CLauncherTextMan::GetInstance().GetText("PROVIDE_FLAG_NAME",0);
		std::vector<std::string> vecString;
		if ( CLauncherTextMan::GetInstance().LoadHelpCommentString(strFlag.GetString(), vecString) )
		{
			size_t i, nSize = vecString.size();
			for ( i = 0; i < nSize; ++i )
			{
				if ( strcmp(vecString[i].c_str(), language::strProvideLanguage[RANPARAM::emProvideLangFlag]) == 0)
				{
					SetWin_Combo_Sel( this, IDC_MNATION_COMBO, i );
				}
			}
		}
		// SetWin_Combo_Sel ( this, IDC_COMBO_NATION, language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
	}
}

void	CAutoPatchManDlg::SetAppPath ()
{
	// Note : 실행파일의 경로를 찾아서 저장한다.
	//
	CString strAppPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		strAppPath = strCommandLine.Left ( strCommandLine.ReverseFind ( '\\' ) );
		
		if ( !strAppPath.IsEmpty() )
		if ( strAppPath.GetAt(0) == '"' )
			strAppPath = strAppPath.Right ( strAppPath.GetLength() - 1 );

        strAppPath += '\\';
		NS_GLOBAL_VAR::strAppPath = strAppPath.GetString();		
	}
	else 
	{
		MessageBox ( "SetAppPath Error", "Error", MB_OK );
		return;
	}

	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );	
	NS_GLOBAL_VAR::strProFile = szPROFILE;
}

int	CAutoPatchManDlg::CheckVersion ()
{
	m_sPatchVer= m_pNetClient->GetPatchVer ();
	m_sGameVer = m_pNetClient->GetGameVer ();
	
	if (m_sPatchVer == E_CHK_VER_NOT_READY)
	{
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 27 ) );
		return E_CHK_VER_NOT_READY; // 버전 전송 실패
	}

	//	NOTE
	//		버전만 확인하고
	//		바로 객체를 삭제함
	//		로그인서버(?)에 계속 붙어있게되는것 방지
	SAFE_DELETE ( m_pNetClient );

	if ( m_sPatchVer == E_CHK_VER_SERVER_UPDATE )
	{
		// 서버 점검중
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 8 ) );
		return E_CHK_VER_SERVER_UPDATE;
	}

    if( m_sPatchVer == E_CHK_VER_NEED_REINSTALL )
    {
        // 버전의 차이가 너무 크다.
        // 재설치가 필요하다.
        ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 79 ) );
        return E_CHK_VER_NEED_REINSTALL;
    }

	//	NOTE
	//		외부 인터페이스에도 버전을 로드하는 부분이 있습니다.
	//		변경될 경우 반드시 체크 해야합니다.
	CString strTemp;
	strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath, g_szClientVerFile );
	FILE* cfp = _fsopen ( strTemp.GetString(), "rb", _SH_DENYNO );
	if( !cfp )
	{
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	if ( 1 != fread ( &m_cPatchVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );	
		
		// 버전 파일 읽기 실패
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	if ( 1 != fread ( &m_cGameVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );	
		
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	fclose ( cfp );

	////////////////////////////////////////////////////////////////////
	//	Launcher 패치
	if ( m_sPatchVer <= m_cPatchVer )	m_emPatchState = E_VERSION_CUR;
	else								m_emPatchState = E_VERSION_UP;
	////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////
	//	게임 데이타 패치
	if ( m_sGameVer <= m_cGameVer )		m_emGameState = E_VERSION_CUR;
	else								m_emGameState = E_VERSION_UP;
	///////////////////////////////////////////////////////////////////

	// 프리다운로드
	// PREDOWNLOADER // 테스트를 위해서 임시로 추가
	//m_emGameState = E_VERSION_UP;
	//////////////////////////////////

	return E_MSG_SUCCEED;	
}