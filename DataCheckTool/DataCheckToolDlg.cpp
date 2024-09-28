// DataCheckToolDlg.cpp : 구현 파일
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


// CDataCheckToolDlg 대화 상자



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


// CDataCheckToolDlg 메시지 처리기

BOOL CDataCheckToolDlg::OnInitDialog()
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


	RECT listRect;
	m_list_SearchResult.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nColWidth[3] = { int(nListWidth*0.2f), int(nListWidth*0.6f), int(nListWidth*0.2f) };

	m_list_SearchResult.SetExtendedStyle ( m_list_SearchResult.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list_SearchResult.InsertColumn( 0, "Use Data", LVCFMT_LEFT, nColWidth[0] );
	m_list_SearchResult.InsertColumn( 1, "File Path", LVCFMT_LEFT, nColWidth[1] );
	m_list_SearchResult.InsertColumn( 2, "File Name", LVCFMT_LEFT, nColWidth[2] );

	CheckRadioButton( IDC_RADIO_WLD, IDC_RADIO_MOBNPC, IDC_RADIO_WLD );

	


	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CDataCheckToolDlg::OnPaint() 
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


// 리스트박스 아이템의 정보를 Draw 하는 함수
void CDataCheckToolDlg::DrawItem(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CRect r(lpDS->rcItem);
	CDC *pDC = CDC::FromHandle(lpDS->hDC);


	// 헤더
	if(parm_push_flag)
	{
		pDC->SetTextColor(0x4080FF);
	}else{
		pDC->SetTextColor(0x915F36);
	}

	int old_mode = pDC->SetBkMode(TRANSPARENT);

	// 리스트박스 아이템에 추가된 문자열을 얻어온다.
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
		"굴림체");

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
		"굴림체");
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

		// 얻어온 문자열을 출력한다.
		//pDC->TextOut(lpDS->rcItem.left + 10, lpDS->rcItem.top + 5, lpBuffer);
		pDC->DrawText( lpBuffer, &drawRC, DT_MODIFYSTRING );
		pDC->SetBkMode(old_mode);

	}


}


// 리스트박스 아이템의 커서를 그리는 함수
void CDataCheckToolDlg::DrawCursor(LPDRAWITEMSTRUCT lpDS, char parm_push_flag)
{
	CDC *pDC = CDC::FromHandle(lpDS->hDC);
	CBrush fill_brush, *p_old_brush = NULL;
	CPen border_pen , *p_old_pen = NULL;
	CRect r(lpDS->rcItem);

	// 헤더
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CDataCheckToolDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if(nIDCtl == IDC_LIST_SEARCH_RESULT){
		// m_list_box : 리스트박스의 객체
		// lpDrawItemStruct->itemID : 리스트박스 아이템의 ID(인덱스)
		// 넘겨온 ID(인덱스)가 리스트박스에 존재하는 ID(인덱스)인가 체크하고
		// 리스트박스에 추가된 아이템이 없다면 Owner Draw 할 것이 없으므로 return;
		if(lpDrawItemStruct->itemID >= (UINT)m_list_SearchResult.GetItemCount() || !m_list_SearchResult.GetItemCount()) return;

		// lpDrawItemStruct->itemAction : 아이템에 발생된 정보
		switch(lpDrawItemStruct->itemAction){
			// 아이템에 포커스 존재하고 선택여부
			case ODA_DRAWENTIRE: case ODA_FOCUS: case ODA_SELECT:

				// Selected 아이템과 Non select 아이템을 구별하여 Draw 한다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
