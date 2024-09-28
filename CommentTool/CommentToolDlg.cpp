// CommentToolDlg.cpp : ���� ����
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

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
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


// CCommentToolDlg ��ȭ ����



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


// CCommentToolDlg �޽��� ó����

BOOL CCommentToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CCommentToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CCommentToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCommentToolDlg::OnLbnSelchangeMenuList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_TYPE_LIST );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;

	m_nTypeIndex = nIndex;

	m_nDataIndex = -1;

	Update();
}

void CCommentToolDlg::OnLbnSelchangeDataList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_DATA_LIST );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex < 0 ) return ;

	m_nDataIndex = nIndex;
}

void CCommentToolDlg::OnLbnDblclkDataList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//
//	CDialog::OnClose();
//}
