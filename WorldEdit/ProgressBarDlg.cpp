// ProgressBarDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include <process.h>

#include "MainFrm.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxLand/ExportXFile/NSExportXFile.h"

#include "ProgressBarDlg.h"


// CProgressBarDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CProgressBarDlg, CDialog)

CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressBarDlg::IDD, pParent)
{

}

CProgressBarDlg::~CProgressBarDlg()
{
}

void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

static	const	int	ID_EXPORT			=	1;

// CProgressBarDlg �޽��� ó�����Դϴ�.

unsigned int WINAPI ExportXFile( LPVOID pParam )
{
	HRESULT hr = S_OK;

	SParam	*pExportParam = (SParam *)pParam;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//	Note : ����� ���� ��� ��������
	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	NSExportXFile::ExportXFile( pView->GetD3DDevice(), pExportParam->m_strExportName.c_str(), pView->GetLandMan() );

	return 0;
}

BOOL CProgressBarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_ctrlProgress.SetRange ( 0, 100 );
	m_ctrlProgress.SetPos ( 0 );

	//	Note	:	0 ������ ����
	ExportProgress::EndPos = 4;
	ExportProgress::CurPos = 0;

	::_beginthreadex(
		NULL,
		0,
		ExportXFile,
		&m_sParam,
		0,
		NULL );

	SetTimer ( ID_EXPORT, 5, NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CProgressBarDlg::Create( const TSTRING& strName )
{
	m_sParam.m_strExportName = strName;
}

void CProgressBarDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	switch ( nIDEvent )
	{
	case ID_EXPORT:
		{			
			if ( ExportProgress::EndPos <= 0 )
			{
				ExportProgress::EndPos = 1;
			}
			int Pos = (int)( ( ExportProgress::CurPos * 100 ) / ExportProgress::EndPos );
			m_ctrlProgress.SetPos ( Pos );

			if ( ExportProgress::CurPos+1 >= ExportProgress::EndPos  )
			{
				KillTimer ( ID_EXPORT );
				EndDialog ( 0 );
			}

			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CProgressBarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam)
			{
			case VK_ESCAPE:
			case VK_RETURN:
				return TRUE;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
