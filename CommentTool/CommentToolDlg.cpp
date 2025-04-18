// CommentToolDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "../SigmaCore/File/StringFile.h"


#include "CommentTool.h"
#include "CommentToolDlg.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "./SetData.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

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


// CCommentToolDlg 대화 상자



CCommentToolDlg::CCommentToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommentToolDlg::IDD, pParent)
	, m_fieldsep( _T(",") )
	, m_nMenuIndex( -1 )
	, m_nTypeIndex( -1 )
	, m_nDataIndex( -1 )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommentToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCommentToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_MENU_LIST, OnLbnSelchangeMenuList)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_LBN_SELCHANGE(IDC_TYPE_LIST, OnLbnSelchangeTypeList)
	ON_LBN_SELCHANGE(IDC_DATA_LIST, OnLbnSelchangeDataList)
	ON_LBN_DBLCLK(IDC_DATA_LIST, OnLbnDblclkDataList)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_INI_TO_LUA, OnFileIniToLua)
//	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCommentToolDlg 메시지 처리기

BOOL CCommentToolDlg::OnInitDialog()
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
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CCommentToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCommentToolDlg::OnPaint() 
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
HCURSOR CCommentToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCommentToolDlg::OnLbnSelchangeMenuList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_MENU_LIST );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;

	m_nMenuIndex = nIndex;
	m_nTypeIndex = -1;

	Update();
}

void CCommentToolDlg::ResetDlg()
{
	{
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_MENU_LIST );
		pListBox->ResetContent();
	}
	
	{
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_TYPE_LIST );
		pListBox->ResetContent();
	}

	for( INT i = 0; i < MAX_EDIT_NUM; i++ )
	{
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_DATA_LIST );
		pListBox->ResetContent();
	}
}

void CCommentToolDlg::Update()
{
	ResetDlg();
	{
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_MENU_LIST );
		pListBox->ResetContent();
		for(int i = 0; i < ( int ) m_vecCommentData.size(); i++ )
		{
			int nIndex = pListBox->AddString ( m_vecCommentData[ i ].strMainKey.c_str() );
			pListBox->SetItemData ( nIndex, i );
		}
		pListBox->SetCurSel( m_nMenuIndex );
	}

	if( m_nMenuIndex >= 0 && ( INT ) m_vecCommentData.size() > m_nMenuIndex )
	{
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_TYPE_LIST );
		pListBox->ResetContent();

		for( INT i= 0; i < ( INT ) m_vecCommentData[ m_nMenuIndex ].vecKeyData.size(); i++ )
		{
			int nIndex = pListBox->AddString ( m_vecCommentData[ m_nMenuIndex ].vecKeyData[ i ].strSubKey.c_str() );
			pListBox->SetItemData ( nIndex, i );
		}
		pListBox->SetCurSel( m_nTypeIndex );
	}

	if( m_nTypeIndex >= 0 && m_nMenuIndex >= 0 )
	{
		CListBox* pListBox = NULL;
		pListBox = (CListBox*) GetDlgItem ( IDC_DATA_LIST );
		pListBox->ResetContent();

		for( INT i = 0; i < ( INT ) m_vecCommentData[ m_nMenuIndex ].vecKeyData[ m_nTypeIndex ].strData.size(); i++ )
		{
			int nIndex = pListBox->AddString ( m_vecCommentData[ m_nMenuIndex ].vecKeyData[ m_nTypeIndex ].strData[ i ].c_str() );
			pListBox->SetItemData ( nIndex, i );
		}
		pListBox->SetCurSel( m_nDataIndex );
	}
}

void CCommentToolDlg::OnLbnSelchangeTypeList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_TYPE_LIST );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;

	m_nTypeIndex = nIndex;

	m_nDataIndex = -1;

	Update();
}

void CCommentToolDlg::OnLbnSelchangeDataList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_DATA_LIST );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;

	m_nDataIndex = nIndex;
}

void CCommentToolDlg::OnLbnDblclkDataList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_DATA_LIST );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;

	m_nDataIndex = nIndex;

	SetDataDlg dlgData;
	dlgData.m_strData = m_vecCommentData[ m_nMenuIndex ].vecKeyData[ m_nTypeIndex ].strData[ m_nDataIndex ];

	if ( IDOK == dlgData.DoModal() )
	{
		m_vecCommentData[ m_nMenuIndex ].vecKeyData[ m_nTypeIndex ].strData[ m_nDataIndex ] = dlgData.m_strData;
		Update();
	}
}

BOOL CCommentToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( pMsg->message == WM_KEYDOWN )
	{
		switch ( pMsg->wParam )
		{
		case VK_RETURN:
		case VK_ESCAPE:
			{
				return true;
			}
			break;
		}
	}


	return CDialog::PreTranslateMessage(pMsg);
}

//void CCommentToolDlg::OnClose()
//{
//	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//
//	CDialog::OnClose();
//}
