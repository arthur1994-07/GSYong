// VerManDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include <share.h>
#include "VerMan.h"
#include "VerManDlg.h"
#include ".\vermandlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVerManDlg 대화 상자



CVerManDlg::CVerManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVerManDlg::IDD, pParent)
	, m_valPatch(_T(""))
	, m_valClient(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVerManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Text(pDX, IDC_EDIT_PATCH, m_valPatch);
	DDX_Text(pDX, IDC_EDIT_CLIENT, m_valClient);
}

BEGIN_MESSAGE_MAP(CVerManDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_DUMMY, OnBnClickedButtonDummy)
END_MESSAGE_MAP()


// CVerManDlg 메시지 처리기

BOOL CVerManDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	DWORD	PatchVer = 0;
	DWORD	ClientVer = 0;

	FILE* fp = _fsopen ( "cVer.bin", "rb", _SH_DENYNO );
	if ( fp )
	{
		if ( 1 != fread ( &PatchVer, sizeof ( DWORD ), 1, fp ) )
		{
			return TRUE;
		}

		if ( 1 != fread ( &ClientVer, sizeof ( DWORD ), 1, fp ) )
		{
			return TRUE;
		}

		fclose ( fp );
	}

	m_valPatch.Format ( "%d", PatchVer );
	m_valClient.Format ( "%d", ClientVer );

	UpdateData ( FALSE );


	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CVerManDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CVerManDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CVerManDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVerManDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData ( TRUE );

	DWORD	PatchVer = atoi ( m_valPatch.GetString () );
	DWORD	ClientVer = atoi ( m_valClient.GetString () );

	FILE* fp = _fsopen ( "cVer.bin", "wb", _SH_DENYNO );
	if ( fp )
	{
		fwrite ( &PatchVer, sizeof ( DWORD ), 1, fp );
		fwrite ( &ClientVer, sizeof ( DWORD ), 1, fp );

		fclose ( fp );
	}
	OnOK();
}

void CVerManDlg::OnBnClickedButtonDummy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	FILE* fp = _fsopen ( "cFileList.bin", "wb", _SH_DENYNO );
	if ( !fp )
	{
		return ;
	}
	
	//	NOTE
	//		첫번째 버전 정보 [아직은 아무런 일도 하지 않음]
	SFILENODE FileInfo;
	FileInfo.Ver = 1;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return ;
	}

	//	NOTE
	//		두번째 파일 개수
	FileInfo.Ver = 1;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return ;
	}

	strcpy ( FileInfo.FileName, "a.bat" );
	strcpy ( FileInfo.SubPath, "\\" );
	FileInfo.Ver = 1;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return ;
	}

	fclose ( fp );
}
