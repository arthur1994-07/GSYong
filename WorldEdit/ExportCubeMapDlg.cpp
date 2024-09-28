// ExportMiniDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include <process.h>
#include <iomanip>

#include "MainFrm.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxTools/CubeMap/NSCubeMapCulling.h"
#include "../EngineLib/DxTools/NsBakeCommon.h"

#include "ExportCubeMapDlg.h"


// CExportCubeMapDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CExportCubeMapDlg, CDialog)

CExportCubeMapDlg::CExportCubeMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportCubeMapDlg::IDD, pParent)
{

}

CExportCubeMapDlg::~CExportCubeMapDlg()
{
}

void CExportCubeMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_EXPORT, m_ctrlButtonStart);
	DDX_Control(pDX, IDC_PROGRESS_EXPORT, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(CExportCubeMapDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CExportCubeMapDlg::OnBnClickedButtonExport)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CExportCubeMapDlg 메시지 처리기입니다.

static	const	int	ID_EXPORT			=	1;

void CExportCubeMapDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 끝이다.
	if ( NSCubeMapCulling::g_nBakeCountCUR > static_cast<int>(NSCubeMapCulling::g_dwBakeCountALL) )
	{
		KillTimer ( ID_EXPORT );
		EndDialog ( 0 );
		return;
	}

	if ( NSCubeMapCulling::g_dwBakeCountALL == 0 )
	{
		CDialog::OnTimer(nIDEvent);
		return;
	}

	std::stringstream strStringStream;
	{
		strStringStream << "현재 진행 갯수 : ";
		strStringStream << NSCubeMapCulling::g_nBakeCountCUR;
		strStringStream << " / 전체 갯수 : ";
		strStringStream << NSCubeMapCulling::g_dwBakeCountALL;
		strStringStream << " - 진행률 : ";
		strStringStream << static_cast<int>( NSCubeMapCulling::g_nBakeCountCUR*100/NSCubeMapCulling::g_dwBakeCountALL );
		strStringStream << "%";
	}

	std::stringstream strStringStreamQD;
	{
		CTimeSpan sTimeSpan;

		//DWORD dwSecond = DxOcclusionQuery::g_dwCullingDelay / 1000;

		//// 딜레이 확인 기능
		//{
		//	strStringStreamQD << "Query Delay : ";
		//	strStringStreamQD << dwSecond;
		//	strStringStreamQD << ".";
		//	strStringStreamQD << std::setw(2) << std::setfill('0') << DxOcclusionQuery::g_dwCullingDelay % 1000 / 10;
		//	strStringStreamQD << "초";

		//	CTimeSpan sTimeSpan( dwSecond );
		//	if ( sTimeSpan.GetMinutes() > 0 )
		//	{
		//		strStringStreamQD << " ( ";

		//		if ( sTimeSpan.GetHours() > 0 )
		//		{
		//			strStringStreamQD << sTimeSpan.GetHours();
		//			strStringStreamQD << "시간 ";
		//		}
		//		if ( sTimeSpan.GetMinutes() > 0 )
		//		{
		//			strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetMinutes();
		//			strStringStreamQD << "분 ";
		//		}

		//		strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetSeconds();
		//		strStringStreamQD << "초 )";
		//	}
		//	strStringStreamQD << "        ";
		//}

		// 진행시간
		{
			CTime sTime = CTime::GetCurrentTime();
			sTime = sTime - CTimeSpan( m_sStartTime.GetTime() );	// 진행된 시간.
			strStringStreamQD << "진행시간 :  ";
			strStringStreamQD << sTime.GetTime();
			strStringStreamQD << "초";

			CTimeSpan sTimeSpan( sTime.GetTime() );
			if ( sTimeSpan.GetMinutes() > 0 )
			{
				strStringStreamQD << " ( ";

				if ( sTimeSpan.GetHours() > 0 )
				{
					strStringStreamQD << sTimeSpan.GetHours();
					strStringStreamQD << "시간 ";
				}
				if ( sTimeSpan.GetMinutes() > 0 )
				{
					strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetMinutes();
					strStringStreamQD << "분 ";
				}

				strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetSeconds();
				strStringStreamQD << "초 )";
			}
		}
	}
	

	DWORD dwEndPos = NSCubeMapCulling::g_dwBakeCountALL;

	CWnd* pWnd = GetDlgItem( IDC_STATIC_TEXT );
	CWnd* pWndQD = GetDlgItem( IDC_STATIC_QUERY_DELAY );
	CWnd* pWndTime = GetDlgItem( IDC_STATIC_TIME );
	CWnd* pWndTimeE = GetDlgItem( IDC_STATIC_TIME_E );
	CWnd* pWndTimeR = GetDlgItem( IDC_STATIC_TIME_R );

	std::stringstream strStringStreamST;
	strStringStreamST << m_sStartTime.GetHour();
	strStringStreamST << ":";
	strStringStreamST << std::setw(2) << std::setfill('0') << m_sStartTime.GetMinute();
	strStringStreamST << ":";
	strStringStreamST << std::setw(2) << std::setfill('0') << m_sStartTime.GetSecond();

	if ( NSCubeMapCulling::g_nBakeCountCUR >= 1 )
	{
		if ( m_nCount != NSCubeMapCulling::g_nBakeCountCUR )
		{
			m_nCount = NSCubeMapCulling::g_nBakeCountCUR;

			CTime sTime = CTime::GetCurrentTime();
			sTime = sTime - CTimeSpan( m_sStartTime.GetTime() );	// 진행된 시간.
			__time64_t dwSecond = sTime.GetTime() * NSCubeMapCulling::g_dwBakeCountALL / NSCubeMapCulling::g_nBakeCountCUR;			

			dwSecond = m_sStartTime.GetHour() * 3600 + m_sStartTime.GetMinute() * 60 + m_sStartTime.GetSecond() + dwSecond;
			CTimeSpan sTimeSpan(dwSecond);


			m_strStringStreamET.str("");
			m_strStringStreamET << sTimeSpan.GetHours();
			m_strStringStreamET << ":";
			m_strStringStreamET << std::setw(2) << std::setfill('0') << sTimeSpan.GetMinutes();
			m_strStringStreamET << ":00";

			// 남은 시간 구하기
			{
				m_strStringStreamRT.str("");

				sTime = CTime::GetCurrentTime();
				dwSecond = dwSecond - sTime.GetHour() * 3600 - sTime.GetMinute() * 60 - sTime.GetSecond();
				CTimeSpan sTimeSpan(dwSecond);
				if ( sTimeSpan.GetHours() > 0 || sTimeSpan.GetMinutes() > 0 )
				{
					m_strStringStreamRT << "남은시간 약 ";

					if ( sTimeSpan.GetHours() > 0 )
					{
						m_strStringStreamRT << sTimeSpan.GetHours();
						m_strStringStreamRT << ":";
						m_strStringStreamRT << std::setw(2) << std::setfill('0') << (sTimeSpan.GetMinutes());
						m_strStringStreamRT << "분";
					}
					else
					{
						m_strStringStreamRT << sTimeSpan.GetMinutes();
						m_strStringStreamRT << "분";
					}
				}
				else
				{
					m_strStringStreamRT << "남은시간 약 ";
					m_strStringStreamRT << sTimeSpan.GetSeconds();
					m_strStringStreamRT << "초";
				}
			}
		}
	}

	switch ( nIDEvent )
	{
	case ID_EXPORT:
		{
			if ( dwEndPos <= 0 )
			{
				dwEndPos = 1;
			}
			int Pos = (int)( ( NSCubeMapCulling::g_nBakeCountCUR * 100 ) / dwEndPos );
			m_ctrlProgress.SetPos ( Pos );

			pWnd->SetWindowText( strStringStream.str().c_str() );
			pWndQD->SetWindowText( strStringStreamQD.str().c_str() );

			pWndTime->SetWindowText( strStringStreamST.str().c_str() );
			pWndTimeE->SetWindowText( m_strStringStreamET.str().c_str() );
			pWndTimeR->SetWindowText( m_strStringStreamRT.str().c_str() );

			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CExportCubeMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

unsigned int WINAPI BakeCubeMapCulling( LPVOID pParam)
{
	HRESULT hr = S_OK;

	SExportParam	*pExportParam = (SExportParam *)pParam;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	DxSetLandMan* pSetLandMan = pView->GetSetLandMan();
	DxLandMan* pLandMan = pView->GetLandMan();
	LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice ();

	if ( !NSCubeMapCulling::Bake( pd3dDevice, 
									pLandMan, 
									pExportParam->m_nSumulationDetailLength, 
									pExportParam->m_nOctreeBoxLength,
									pExportParam->m_dwCubeMapSize ) ) 
	{
		NSCubeMapCulling::g_nBakeCountCUR = static_cast<int>(NSCubeMapCulling::g_dwBakeCountALL) + 1;
		return 0;
	}

	CTime Time = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format ( "%d_%02d_%02d_%02d_%02d_%02d.wld", Time.GetYear(), Time.GetMonth(), Time.GetDay(),
		Time.GetHour(), Time.GetMinute(), Time.GetSecond() );
	CString strBackUp(DxLandMan::GetPath());
	strBackUp += pLandMan->GetNameWLD().c_str();
	strBackUp += _T("_");
	strBackUp += strTime.GetString();

	// 
	NSCubeMapCulling::Save( pLandMan->GetMakeTimeWLD(), strBackUp.GetString() );			// BackUp

	if ( pLandMan->GetNameWLD().empty() )
	{
		AfxMessageBox("기초가 되는 wld 이름이 없습니다. 임시로 cbm 파일을 만듭니다.");

		CString szFilter = "CubeMap ( *.cbm ) |*.cbm|";
		CFileDialog dlg ( FALSE, ".cbm", NULL,
			OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
			(CWorldEditView*)pView );

		dlg.m_ofn.lpstrInitialDir = DxLandMan::GetPath ();
		if ( dlg.DoModal() == IDOK )
		{
			CString WriteName = dlg.GetPathName();

			strBackUp = WriteName.GetString();
			strBackUp += pLandMan->GetNameWLD().c_str();

			NSCubeMapCulling::Save( pLandMan->GetMakeTimeWLD(), strBackUp.GetString() ); // 지형
		}
	}
	else
	{
		strBackUp = DxLandMan::GetPath();
		strBackUp += pLandMan->GetNameWLD().c_str();

		NSCubeMapCulling::Save( pLandMan->GetMakeTimeWLD(), strBackUp.GetString() ); // 지형
	}

	++NSCubeMapCulling::g_nBakeCountCUR;
	return 0;
}

void CExportCubeMapDlg::OnBnClickedButtonExport()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ctrlButtonStart.EnableWindow ( FALSE );

	m_ctrlProgress.SetRange ( 0, 100 );
	m_ctrlProgress.SetPos ( 0 );

	// 타이머 돌기 전에 값을 미리 초기화 해 놓는다.
	NSCubeMapCulling::g_nBakeCountCUR = 0;

	SetTimer ( ID_EXPORT, 10, NULL );

	m_sStartTime = CTime::GetCurrentTime();

	::_beginthreadex(
			NULL,
			0,
			BakeCubeMapCulling,
			&m_SParam,
			0,
			NULL );
}

void CExportCubeMapDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnClose();
}

void CExportCubeMapDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// Bake 중인데 껐다면 
	if ( !NSBakeCommon::g_bForceDisable )
	{
		NSBakeCommon::g_bForceDisable = TRUE;

		// 작업이 완료가 안된 상황이라면, 붙잡아놓는다.
		while ( NSCubeMapCulling::g_nBakeCountCUR <= static_cast<int>(NSCubeMapCulling::g_dwBakeCountALL) )
		{
			Sleep(1);
		}
	}
}
