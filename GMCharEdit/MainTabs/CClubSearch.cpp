// CClubSearch.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../MainFrm.h"
#include "../GMCharEdit.h"
#include "CClubSearch.h"

#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CClubSearch

IMPLEMENT_DYNCREATE(CClubSearch, CFormView)

CClubSearch::CClubSearch()
	: CFormView(CClubSearch::IDD)
	, m_strClubName(_T(""))
	, m_bInit(FALSE)
{
}

CClubSearch::~CClubSearch()
{
}

void CClubSearch::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CLUB_NAME, m_strClubName);
	DDX_Control(pDX, IDC_LIST_CLUB_SEARCH, m_ListClubSearch);
}

BEGIN_MESSAGE_MAP(CClubSearch, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_CLUB_SEARCH, OnBnClickedButtonClubSearch)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CClubSearch 진단입니다.

#ifdef _DEBUG
void CClubSearch::AssertValid() const
{
	CFormView::AssertValid();
}

void CClubSearch::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CClubSearch 메시지 처리기입니다.

void CClubSearch::OnBnClickedButtonClubSearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CClubSearch::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_ListClubSearch.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	InitListHead();
	m_bInit = TRUE;
}

void CClubSearch::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CClubSearch::InitListHead()
{
	LVCOLUMN Col;
	
	// Num, Name, Class, Tribe, Level,	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= _T("Club Num");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListClubSearch.InsertColumn(0, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;
	Col.pszText		= _T("Club Name");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListClubSearch.InsertColumn(1, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Master Num");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListClubSearch.InsertColumn(2, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Master Name");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListClubSearch.InsertColumn(3, &Col);
    
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Rank");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListClubSearch.InsertColumn(4, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Club Money");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListClubSearch.InsertColumn(5, &Col);
}

