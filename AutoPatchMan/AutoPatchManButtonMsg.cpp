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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.    
	m_MouseClickPos = point;
	m_bLBDown = TRUE;
	SetCapture();

	CDialog::OnLButtonDown(nFlags, point);
}


void CAutoPatchManDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
		if (RANPARAM::bAllMinimum) // �ּ�ȭ �ɼ� ���� �������� ��� â �ּ�ȭ. by luxes.
            ::SendMessage(hTrayWnd, WM_COMMAND, 419, 0);
	}

    EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
    if (sp == SP_INDONESIA)
        GameStartIndenesia();
    else
        GameStart();
	OnOK(); // ��ó ����
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

        //	96byte ¥�� cmd�� terra�� ActiveX���� ���޵Ǵ� �α��� ����.
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
            //#else // �����̽þ�
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

			// �ѱ�, �ѱ��׼� ���α��� ����ε� ��ó�� 1.5�� �����ϴ� ���� ������ �����ʾ�
			// �Ʒ��� ������ �߰��� khkim
			CDaumGameParameter cDGP;
			if ( cDGP.Set ( "Parameter", szParameter ) == FALSE )
			{
				return;
			}
        }
        //#endif // !KR_PARAM

		int RetExcute = 0;

		// ���� �ٿ�δ� ����
		if( NS_GLOBAL_VAR::g_bNeedPreDownloader )
		{
			// ���� �ٿ�ε� ���࿩�θ� ����� �����ϴ� ��쿡�� ����
			if( MessageBox("���� �ٿ�ε� �� ������ ������Ʈ�� �����ϴ��� Ȯ���Ͻðڽ��ϱ�?", "Notice", MB_OKCANCEL ) == IDOK )
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

		// �����ٿ�ε�
		// �ӽ÷� ���� �������� �ʵ��� ����
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
	//	���� ���� �Ķ��Ÿ�� ������ �����ݴϴ�.

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

	// �����ϰ� �ֽ��ϴ�.
	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 3 ));
	OnOK();
}

void CAutoPatchManDlg::OnBnClickedButtonOption()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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
	//	dlg.m_strAreaName[5] = RANPARAM::China_Region[5].strName; // �߱����� �߰�
	}
//#endif

	dlg.DoModal ();
}

void CAutoPatchManDlg::OnBnClickedButtonRetry()
{
	//	��ư ����
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
//	m_ctrlOption.EnableWindow( FALSE );	�ӽ�;

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