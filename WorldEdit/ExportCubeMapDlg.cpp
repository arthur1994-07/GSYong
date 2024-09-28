// ExportMiniDlg.cpp : ���� �����Դϴ�.
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


// CExportCubeMapDlg ��ȭ �����Դϴ�.

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


// CExportCubeMapDlg �޽��� ó�����Դϴ�.

static	const	int	ID_EXPORT			=	1;

void CExportCubeMapDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	// ���̴�.
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
		strStringStream << "���� ���� ���� : ";
		strStringStream << NSCubeMapCulling::g_nBakeCountCUR;
		strStringStream << " / ��ü ���� : ";
		strStringStream << NSCubeMapCulling::g_dwBakeCountALL;
		strStringStream << " - ����� : ";
		strStringStream << static_cast<int>( NSCubeMapCulling::g_nBakeCountCUR*100/NSCubeMapCulling::g_dwBakeCountALL );
		strStringStream << "%";
	}

	std::stringstream strStringStreamQD;
	{
		CTimeSpan sTimeSpan;

		//DWORD dwSecond = DxOcclusionQuery::g_dwCullingDelay / 1000;

		//// ������ Ȯ�� ���
		//{
		//	strStringStreamQD << "Query Delay : ";
		//	strStringStreamQD << dwSecond;
		//	strStringStreamQD << ".";
		//	strStringStreamQD << std::setw(2) << std::setfill('0') << DxOcclusionQuery::g_dwCullingDelay % 1000 / 10;
		//	strStringStreamQD << "��";

		//	CTimeSpan sTimeSpan( dwSecond );
		//	if ( sTimeSpan.GetMinutes() > 0 )
		//	{
		//		strStringStreamQD << " ( ";

		//		if ( sTimeSpan.GetHours() > 0 )
		//		{
		//			strStringStreamQD << sTimeSpan.GetHours();
		//			strStringStreamQD << "�ð� ";
		//		}
		//		if ( sTimeSpan.GetMinutes() > 0 )
		//		{
		//			strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetMinutes();
		//			strStringStreamQD << "�� ";
		//		}

		//		strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetSeconds();
		//		strStringStreamQD << "�� )";
		//	}
		//	strStringStreamQD << "        ";
		//}

		// ����ð�
		{
			CTime sTime = CTime::GetCurrentTime();
			sTime = sTime - CTimeSpan( m_sStartTime.GetTime() );	// ����� �ð�.
			strStringStreamQD << "����ð� :  ";
			strStringStreamQD << sTime.GetTime();
			strStringStreamQD << "��";

			CTimeSpan sTimeSpan( sTime.GetTime() );
			if ( sTimeSpan.GetMinutes() > 0 )
			{
				strStringStreamQD << " ( ";

				if ( sTimeSpan.GetHours() > 0 )
				{
					strStringStreamQD << sTimeSpan.GetHours();
					strStringStreamQD << "�ð� ";
				}
				if ( sTimeSpan.GetMinutes() > 0 )
				{
					strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetMinutes();
					strStringStreamQD << "�� ";
				}

				strStringStreamQD << std::setw(2) << std::setfill('0') << sTimeSpan.GetSeconds();
				strStringStreamQD << "�� )";
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
			sTime = sTime - CTimeSpan( m_sStartTime.GetTime() );	// ����� �ð�.
			__time64_t dwSecond = sTime.GetTime() * NSCubeMapCulling::g_dwBakeCountALL / NSCubeMapCulling::g_nBakeCountCUR;			

			dwSecond = m_sStartTime.GetHour() * 3600 + m_sStartTime.GetMinute() * 60 + m_sStartTime.GetSecond() + dwSecond;
			CTimeSpan sTimeSpan(dwSecond);


			m_strStringStreamET.str("");
			m_strStringStreamET << sTimeSpan.GetHours();
			m_strStringStreamET << ":";
			m_strStringStreamET << std::setw(2) << std::setfill('0') << sTimeSpan.GetMinutes();
			m_strStringStreamET << ":00";

			// ���� �ð� ���ϱ�
			{
				m_strStringStreamRT.str("");

				sTime = CTime::GetCurrentTime();
				dwSecond = dwSecond - sTime.GetHour() * 3600 - sTime.GetMinute() * 60 - sTime.GetSecond();
				CTimeSpan sTimeSpan(dwSecond);
				if ( sTimeSpan.GetHours() > 0 || sTimeSpan.GetMinutes() > 0 )
				{
					m_strStringStreamRT << "�����ð� �� ";

					if ( sTimeSpan.GetHours() > 0 )
					{
						m_strStringStreamRT << sTimeSpan.GetHours();
						m_strStringStreamRT << ":";
						m_strStringStreamRT << std::setw(2) << std::setfill('0') << (sTimeSpan.GetMinutes());
						m_strStringStreamRT << "��";
					}
					else
					{
						m_strStringStreamRT << sTimeSpan.GetMinutes();
						m_strStringStreamRT << "��";
					}
				}
				else
				{
					m_strStringStreamRT << "�����ð� �� ";
					m_strStringStreamRT << sTimeSpan.GetSeconds();
					m_strStringStreamRT << "��";
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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
		AfxMessageBox("���ʰ� �Ǵ� wld �̸��� �����ϴ�. �ӽ÷� cbm ������ ����ϴ�.");

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

			NSCubeMapCulling::Save( pLandMan->GetMakeTimeWLD(), strBackUp.GetString() ); // ����
		}
	}
	else
	{
		strBackUp = DxLandMan::GetPath();
		strBackUp += pLandMan->GetNameWLD().c_str();

		NSCubeMapCulling::Save( pLandMan->GetMakeTimeWLD(), strBackUp.GetString() ); // ����
	}

	++NSCubeMapCulling::g_nBakeCountCUR;
	return 0;
}

void CExportCubeMapDlg::OnBnClickedButtonExport()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ctrlButtonStart.EnableWindow ( FALSE );

	m_ctrlProgress.SetRange ( 0, 100 );
	m_ctrlProgress.SetPos ( 0 );

	// Ÿ�̸� ���� ���� ���� �̸� �ʱ�ȭ �� ���´�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CDialog::OnClose();
}

void CExportCubeMapDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	// Bake ���ε� ���ٸ� 
	if ( !NSBakeCommon::g_bForceDisable )
	{
		NSBakeCommon::g_bForceDisable = TRUE;

		// �۾��� �Ϸᰡ �ȵ� ��Ȳ�̶��, ����Ƴ��´�.
		while ( NSCubeMapCulling::g_nBakeCountCUR <= static_cast<int>(NSCubeMapCulling::g_dwBakeCountALL) )
		{
			Sleep(1);
		}
	}
}
