// CClubSearch.cpp : ���� �����Դϴ�.
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


// CClubSearch �����Դϴ�.

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


// CClubSearch �޽��� ó�����Դϴ�.

void CClubSearch::OnBnClickedButtonClubSearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CClubSearch::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_ListClubSearch.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	InitListHead();
	m_bInit = TRUE;
}

void CClubSearch::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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

