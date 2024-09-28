// DataCheckToolDlg.cpp : ���� ����
//

#include "stdafx.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/Common/SUBPATH.h"

#include "../RanLogic/GLStringTable.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "../EngineLib/DxLand/DxLandMan.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxMeshs/FrameMesh/DxReplaceContainer.h"

#include ".\worlddatacheck.h"

#include "DataCheckTool.h"
#include "DataCheckToolDlg.h"
#include ".\datachecktooldlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CDataCheckToolDlg ��ȭ ����



CDataCheckToolDlg::CDataCheckToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataCheckToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pPath = new SUBPATH(SP_OFFICE_TEST);
}

CDataCheckToolDlg::~CDataCheckToolDlg()
{
    delete m_pPath;
    m_pPath = NULL;
}

void CDataCheckToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEARCH_RESULT, m_list_SearchResult);
}

BEGIN_MESSAGE_MAP(CDataCheckToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SEARCH_RESULT, OnNMCustomdrawListSearchResult)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
END_MESSAGE_MAP()


// CDataCheckToolDlg �޽��� ó����

BOOL CDataCheckToolDlg::OnInitDialog()
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


	RECT listRect;
	m_list_SearchResult.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nColWidth[3] = { int(nListWidth*0.2f), int(nListWidth*0.6f), int(nListWidth*0.2f) };

	m_list_SearchResult.SetExtendedStyle ( m_list_SearchResult.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_SearchResult.InsertColumn( 0, "Use Data", LVCFMT_LEFT, nColWidth[0] );
	m_list_SearchResult.InsertColumn( 1, "File Path", LVCFMT_LEFT, nColWidth[1] );
	m_list_SearchResult.InsertColumn( 2, "File Name", LVCFMT_LEFT, nColWidth[2] );

	CheckRadioButton( IDC_RADIO_WLD, IDC_RADIO_MOBNPC, IDC_RADIO_WLD );

	


	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CDataCheckToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDataCheckToolDlg::OnPaint() 
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
HCURSOR CDataCheckToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString	CDataCheckToolDlg::GetAppPath()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if (!strCommandLine.IsEmpty())
	{
		DWORD dwFind = strCommandLine.ReverseFind('\\');
		if (dwFind != -1)
		{
			strFullPath = strCommandLine.Left(dwFind);

			if (!strFullPath.IsEmpty())
				if (strFullPath.GetAt(0) == '"')
					strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
		}
	}
	return strFullPath;
}


// ����Ʈ�ڽ� �������� ������ Draw �ϴ� �Լ�
void CDataCheckToolDlg::DrawItem(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CRect r(lpDS->rcItem);
	CDC *pDC = CDC::FromHandle(lpDS->hDC);


	// ���
	if(parm_push_flag)
	{
		pDC->SetTextColor(0x4080FF);
	}else{
		pDC->SetTextColor(0x915F36);
	}

	int old_mode = pDC->SetBkMode(TRANSPARENT);

	// ����Ʈ�ڽ� �����ۿ� �߰��� ���ڿ��� ���´�.
	TCHAR  lpBuffer[256];
	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM ;
	lvi.iItem = lpDS->itemID ; 	
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer ;
	lvi.cchTextMax = sizeof(lpBuffer);
	m_list_SearchResult.GetItem(&lvi);

	LV_COLUMN lvc, lvcprev;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;

	RECT rc;
	rc = lpDS->rcItem;
	rc.top += 3;

	CFont fontMain, fontSub;
	fontMain.CreateFont(16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"����ü");

	fontSub.CreateFont(14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"����ü");
	for ( int nCol=0; m_list_SearchResult.GetColumn(nCol, &lvc); nCol++)
	{
		if ( nCol > 0 ) 
		{
			// Get Previous Column Width in order to move the next display item
			m_list_SearchResult.GetColumn(nCol-1, &lvcprev) ;
			rc.left += lvcprev.cx;
		}

		rc.right = rc.left + lvc.cx;

		// Get the text 
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDS->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		m_list_SearchResult.GetItem(&lvi);

		RECT drawRC = rc;
		drawRC.left  += 3;
		drawRC.right -= 3;

		if( nCol < 4 )
		{
			pDC->SelectObject(&fontMain);
		}else{
			pDC->SelectObject(&fontSub);
		}

		// ���� ���ڿ��� ����Ѵ�.
		//pDC->TextOut(lpDS->rcItem.left + 10, lpDS->rcItem.top + 5, lpBuffer);
		pDC->DrawText( lpBuffer, &drawRC, DT_MODIFYSTRING );
		pDC->SetBkMode(old_mode);

	}


}


// ����Ʈ�ڽ� �������� Ŀ���� �׸��� �Լ�
void CDataCheckToolDlg::DrawCursor(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CDC *pDC = CDC::FromHandle(lpDS->hDC);
	CBrush fill_brush, *p_old_brush = NULL;
	CPen border_pen , *p_old_pen = NULL;
	CRect r(lpDS->rcItem);

	// ���
	if(parm_push_flag)
	{
		border_pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		fill_brush.CreateSolidBrush(0xD2D3EF);
	}else{
		if( lpDS->itemID % 3 == 0 )
		{
			border_pen.CreatePen(PS_SOLID, 3, 0xD2D3EF);
			fill_brush.CreateSolidBrush(0xEEDFD3);
		}else{
			border_pen.CreatePen(PS_SOLID, 0, 0xEEDFD3);
			fill_brush.CreateSolidBrush(RGB(255, 255, 255));
		}
	}

	p_old_pen = pDC->SelectObject(&border_pen);
	p_old_brush = pDC->SelectObject(&fill_brush);

	pDC->Rectangle(r);
	pDC->SelectObject(p_old_brush);
	pDC->SelectObject(p_old_pen);

	fill_brush.DeleteObject();
	border_pen.DeleteObject();
}

void CDataCheckToolDlg::OnNMCustomdrawListSearchResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}

void CDataCheckToolDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if(nIDCtl == IDC_LIST_SEARCH_RESULT){
		// m_list_box : ����Ʈ�ڽ��� ��ü
		// lpDrawItemStruct->itemID : ����Ʈ�ڽ� �������� ID(�ε���)
		// �Ѱܿ� ID(�ε���)�� ����Ʈ�ڽ��� �����ϴ� ID(�ε���)�ΰ� üũ�ϰ�
		// ����Ʈ�ڽ��� �߰��� �������� ���ٸ� Owner Draw �� ���� �����Ƿ� return;
		if(lpDrawItemStruct->itemID >= (UINT)m_list_SearchResult.GetItemCount() || !m_list_SearchResult.GetItemCount()) return;

		// lpDrawItemStruct->itemAction : �����ۿ� �߻��� ����
		switch(lpDrawItemStruct->itemAction){
			// �����ۿ� ��Ŀ�� �����ϰ� ���ÿ���
			case ODA_DRAWENTIRE: case ODA_FOCUS: case ODA_SELECT:

				// Selected �����۰� Non select �������� �����Ͽ� Draw �Ѵ�.
				if(lpDrawItemStruct->itemState & ODS_SELECTED){
					DrawCursor(lpDrawItemStruct, 1);
					DrawItem(lpDrawItemStruct, 1);

				} else {
					DrawCursor(lpDrawItemStruct, 0);
					DrawItem(lpDrawItemStruct, 0);
					break;
				}
		}
	} else CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

HRESULT CDataCheckToolDlg::SearchWldData()
{
	HANDLE handle;
	WIN32_FIND_DATA fd;
	char fpath[300];
	char fOpenName[256];
	BOOL Result = TRUE;
    std::vector<std::string> vecFileName;

	sprintf(fpath,"%s\\%s", GetAppPath(), m_pPath->MapFile().c_str()); //SUBPATH::MAP_FILE);	
	sprintf(fOpenName,"%s\\*.wld",fpath);

	handle = FindFirstFile(fOpenName,&fd);

	while(Result)
	{
		if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if( strstr( fd.cFileName, "wld0" ) == NULL )
			{
				vecFileName.push_back( fd.cFileName );
			}
		}
		Result = FindNextFile(handle,&fd);
	}
 
	CWorldDataCheck worldDataCheck;

	char szTempChar[256] = {0};
	DxLandMan testLandMan;
	for( int i = 0; i < vecFileName.size(); i++ )
	{
		sprintf( szTempChar, "%s%s", fpath, vecFileName[i].c_str() );
		worldDataCheck.LoadFile( szTempChar );
	}
	

	return S_OK;
}

HRESULT CDataCheckToolDlg::SearchItemData()
{
	return S_OK;
}

HRESULT CDataCheckToolDlg::SearchMobNpcData()
{
	return S_OK;
}

void CDataCheckToolDlg::OnBnClickedButtonSearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GetWin_Check( this, IDC_RADIO_WLD ) )
	{
		SearchWldData();
	}else if( GetWin_Check( this, IDC_RADIO_ITEM ) )
	{
		SearchItemData();
	}else if( GetWin_Check( this, IDC_RADIO_MOBNPC ) )
	{
		SearchMobNpcData();
	}

}
