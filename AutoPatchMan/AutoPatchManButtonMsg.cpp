#include "stdafx.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Util/SystemInfo.h"

#include "../MfcExLib/DaumParam/DaumGameParameter.h"
#include "../MfcExLib/rol_clipboard.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "VtcGuardLauncher.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"
#include "GlobalVariable.h"
#include "LoginThread.h"
#include "FtpThread.h"
#include "GameSetDlg.h"
#include "LogControl.h"
#include "LauncherText.h"
#include "DlgWarningWhql.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char szParameter[MAX_DGPARAMETER_LENGTH];

void CAutoPatchManDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.    
	m_MouseClickPos = point;
	m_bLBDown = TRUE;
	SetCapture();

	CDialog::OnLButtonDown(nFlags, point);
}


void CAutoPatchManDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( !m_bLBDown ) return;

	RECT rect;
	GetWindowRect ( &rect );

	CPoint MovePos;
	MovePos.x = m_MouseClickPos.x - point.x;
	MovePos.y = m_MouseClickPos.y - point.y;
	
	rect.left -= MovePos.x;
	rect.top -= MovePos.y;
	rect.right -= MovePos.x;
	rect.bottom -= MovePos.y;

	MoveWindow ( &rect );

	CDialog::OnMouseMove(nFlags, point);
}

void CAutoPatchManDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( m_bLBDown )
	{
		ReleaseCapture ();
		m_bLBDown = FALSE;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CAutoPatchManDlg::OnBnClickedButtonStart()
{
	CWinApp* pApp = AfxGetApp();
	if (!pApp)
        return;

	HWND hTrayWnd = ::FindWindow("Shell_TrayWnd", NULL);	
	if (hTrayWnd)
	{
		if (RANPARAM::bAllMinimum) // 최소화 옵션 켜져 있을때만 모든 창 최소화. by luxes.
            ::SendMessage(hTrayWnd, WM_COMMAND, 419, 0);
	}

    EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
    if (sp == SP_INDONESIA)
        GameStartIndenesia();
    else
        GameStart();
	OnOK(); // 런처 종료
}

void CAutoPatchManDlg::GameStartIndenesia() 
{
    char szMsg[MAX_PATH] = {0};
    std::string RunParam(
        sc::string::format(
        //"%1%\\game.exe#gameparam#KJSHDF8932N3B2M3NBCPS8DHLO63G9FV", sc::getAppPath()));
        "%1%\\game.exe#/app_run#KJSHDF8932N3B2M3NBCPS8DHLO63G9FV", sc::getAppPath()));
    vt_run(RunParam.c_str(), szMsg);
}

void CAutoPatchManDlg::GameStart()
{
    CWinApp* pApp = AfxGetApp();
    EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
    CString StrCmdLine = pApp->m_lpCmdLine;

    STRUTIL::ClearSeparator();
    STRUTIL::RegisterSeparator("/");
    STRUTIL::RegisterSeparator(" ");

    CStringArray strCmdArray;
    STRUTIL::StringSeparate(StrCmdLine, strCmdArray);

    CString strCommand = CString("app") + _T("_") + _T("run") + _T(" ");
    CString strNewCmd = _T("/") + strCommand;

    for (int i=0; i<strCmdArray.GetCount(); ++i)
    {
        CString strCmd = strCmdArray.GetAt(i);
        CString strNew;

        //	96byte 짜리 cmd는 terra의 ActiveX에서 전달되는 로그인 정보.
        //#if defined (PH_PARAM) || defined ( VN_PARAM)
        if (sp == SP_PHILIPPINES || sp == SP_VIETNAM)
        {
            if (strCmd.GetLength() == 96)
                strNew = _T("TID:") + strCmd;
            else
                strNew = strCmd;
        }
        else
        {
            //#else // 말레이시아
            if (strCmd.GetLength() == 92)
                strNew = _T("TID:") + strCmd;
            else
                strNew = strCmd;
        }
        //#endif

        strNewCmd += _T("/") + strNew + _T(" ");
    }

    {
        //#ifndef KR_PARAM
        if (sp == SP_KOREA || sp == SP_KOREA_TEST)
        {
            rol_clipboard::InitClipboard();
            rol_clipboard::PutClipboard(m_hWnd);

			// 한국, 한국테섭 웹로그인 방식인데 런처를 1.5로 빌드하니 게임 실행이 되지않아
			// 아래의 내용을 추가함 khkim
			CDaumGameParameter cDGP;
			if ( cDGP.Set ( "Parameter", szParameter ) == FALSE )
			{
				return;
			}
        }
        //#endif // !KR_PARAM

		int RetExcute = 0;

		// 사전 다운로더 실행
		if( NS_GLOBAL_VAR::g_bNeedPreDownloader )
		{
			// 사전 다운로드 실행여부를 물어보고 승인하는 경우에만 동작
			if( MessageBox("사전 다운로드 된 파일의 업데이트가 존재하는지 확인하시겠습니까?", "Notice", MB_OKCANCEL ) == IDOK )
			{
				CString strPreDownloader;
				strPreDownloader.Format("\"%s%s\"", NS_GLOBAL_VAR::strAppPath, NS_GLOBAL_VAR::g_szPreDownloaderName);	
				RetExcute = (int) ShellExecute(m_hWnd, "open", strPreDownloader.GetString(), strNewCmd.GetString(), NULL, SW_SHOW);

				if (RetExcute < 32)
				{
					CString strMsg;
		#ifndef _RELEASE
					strMsg.Format(
						"%s PreDownloader Error Number : %d %s",
						ID2LAUNCHERTEXT("IDS_MESSAGE", 2),
						RetExcute,
						strNewCmd.GetString()); 
		#else
					strMsg.Format(
						"%s PreDownloader Error Number : %d",
						ID2LAUNCHERTEXT("IDS_MESSAGE", 2),
						result);
		#endif
					MessageBox(strMsg.GetString(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}

		// 프리다운로드
		// 임시로 게임 시작하지 않도록 막음
        CString str;
        str.Format("\"%s%s\"", NS_GLOBAL_VAR::strAppPath, NS_GLOBAL_VAR::g_szRanFileName);	
        RetExcute = (int) ShellExecute(m_hWnd, "open", str.GetString(), strNewCmd.GetString(), NULL, SW_SHOW);
        if (RetExcute < 32)
        {
            CString strMsg;
#ifndef _RELEASE
            strMsg.Format(
                "%s Error Number : %d %s",
                ID2LAUNCHERTEXT("IDS_MESSAGE", 2),
                RetExcute,
                strNewCmd.GetString()); 
#else
            strMsg.Format(
                "%s Error Number : %d",
                ID2LAUNCHERTEXT("IDS_MESSAGE", 2),
                result);
#endif
            MessageBox(strMsg.GetString(), "ERROR", MB_OK | MB_ICONEXCLAMATION);

            //#ifdef KR_PARAM
            if (sp == SP_KOREA || sp == SP_KOREA_TEST)
            {
                CDaumGameParameter cDGP;
                cDGP.Empty();
            }
            //#endif	// KR_PARAM
        }
    }
}

void CAutoPatchManDlg::OnBnClickedButtonExit()
{
	//	다음 게임 파라메타의 내용을 지워줍니다.

	//#ifdef KR_PARAM
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	if (sp == SP_KOREA || sp == SP_KOREA_TEST)
	{
		CDaumGameParameter cDGP;
		cDGP.Empty();
	}
	//#endif	// KR_PARAM

	m_bForceTerminate = TRUE;

	EndThread( m_pWebBrowserThread );
	EndThread( m_pLoginThread );
	EndThread( m_pPatchThread );
	EndThread( m_pHashThread );

	// 종료하고 있습니다.
	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 3 ));
	OnOK();
}

void CAutoPatchManDlg::OnBnClickedButtonOption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CGameSetDlg dlg;

//#ifdef CH_PARAM
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	if (sp == SP_CHINA)
	{
		dlg.m_nDownloadArea = RANPARAM::nChinaRegion;
		dlg.m_strAreaName[0] = RANPARAM::China_Region[0].strName;
		dlg.m_strAreaName[1] = RANPARAM::China_Region[1].strName;
		dlg.m_strAreaName[2] = RANPARAM::China_Region[2].strName;
	//	dlg.m_strAreaName[3] = RANPARAM::China_Region[3].strName;
	//	dlg.m_strAreaName[4] = RANPARAM::China_Region[4].strName;
	//	dlg.m_strAreaName[5] = RANPARAM::China_Region[5].strName; // 중국지역 추가
	}
//#endif

	dlg.DoModal ();
}

void CAutoPatchManDlg::OnBnClickedButtonRetry()
{
	//	버튼 막기
//#ifdef CH_PARAM
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	if (sp == SP_CHINA)
	{
		::ShowWindow( m_ctrlRetry.GetSafeHwnd(), SW_SHOW );
		::ShowWindow( m_ctrlStart.GetSafeHwnd(), SW_HIDE );
		::ShowWindow( m_ctrlUpdate.GetSafeHwnd(), SW_HIDE );
		m_ctrlRetry.EnableWindow( FALSE );
	}
//#endif


	m_ctrlStart.EnableWindow( FALSE );
	m_ctrlUpdate.EnableWindow( FALSE );
//	m_ctrlOption.EnableWindow( FALSE );	임시;

    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();

    RANPARAM::LOAD(
        NS_GLOBAL_VAR::strAppPath.GetString(),
        pApp->GetServiceProvider(),
        false,
        false);

	m_ctrlHomePage.SetURL( RANPARAM::HomePageAddress );

	EndThread( m_pLoginThread );
	EndThread( m_pPatchThread );
	EndThread( m_pHashThread );

	if( !m_pNetClient )
	{
		m_pNetClient = new CNetClient( m_hWnd );
		if( !m_pNetClient )
		{
			EndDialog(IDOK);
			return;
		}
	}

	m_nLoginTry = 0;
	BEGIN_LOGIN_THREAD ();
}