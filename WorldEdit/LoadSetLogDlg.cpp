// LoadSetLogDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "LoadSetLogDlg.h"
#include "../EngineLib/Common/SUBPATH.H"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CLoadSetLogDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLoadSetLogDlg, CDialog)
CLoadSetLogDlg::CLoadSetLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadSetLogDlg::IDD, pParent)
	, m_strEditLog(_T(""))
{
}

CLoadSetLogDlg::~CLoadSetLogDlg()
{
}

void CLoadSetLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOADSET_LOG, m_ctrlEditLog);
	DDX_Text(pDX, IDC_EDIT_LOADSET_LOG, m_strEditLog);
}


BEGIN_MESSAGE_MAP(CLoadSetLogDlg, CDialog)
END_MESSAGE_MAP()


// CLoadSetLogDlg �޽��� ó�����Դϴ�.

void CLoadSetLogDlg::LoadSetLog(void)
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	TCHAR szFullPath[MAX_PATH]={0};

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	SHGetSpecialFolderPath( NULL, szFullPath, CSIDL_PERSONAL, FALSE );
	StringCchCat( szFullPath, MAX_PATH, pPath->DebugInfoRoot().c_str() ); // SUBPATH::DEBUGINFO_ROOT );
	StringCchCat( szFullPath, MAX_PATH, _T("LoadSet.log") );
	
	FILE	*fp = NULL;
	fp = _fsopen ( szFullPath, "rb", _SH_DENYNO );
    if ( !fp )
	{
		MessageBox ( "LoadSet.log ������ ã�� �� �����ϴ�." );
		return;
	}	
	
	int		FILESIZE = 0;
	int		spos = 0, epos = 0;

	spos = ftell ( fp );
	fseek ( fp, 0, SEEK_END );
	epos = ftell ( fp );
	FILESIZE = epos - spos;
	fseek ( fp, 0, SEEK_SET );

	int		rValue = 0;
	char	*str = new char [ FILESIZE + 1 ];
	memset ( str, 0, sizeof ( char ) * ( FILESIZE + 1 ) );

	rValue = fread ( str, FILESIZE, sizeof ( char ), fp );
	if ( rValue != sizeof ( char ) )
	{
		MessageBox ( "LoadSet.log ���� �б⿡ �����Ͽ����ϴ�." );
		delete [] str;
		str = NULL;
		return;
	}

	m_strEditLog = str;

	UpdateData ( FALSE );

	delete [] str;
	str = NULL;

	fclose ( fp );
}

BOOL CLoadSetLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	LoadSetLog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
