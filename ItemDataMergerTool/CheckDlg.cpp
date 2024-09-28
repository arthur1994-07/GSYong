// CheckDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Item/GLItem.h"

#include "ItemDataMergerTool.h"
#include "IDCheckDlg.h"
#include "CheckDlg.h"


// CCheckDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCheckDlg, CDialog)

CCheckDlg::CCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckDlg::IDD, pParent)
{
	m_vecIDList.clear();
}

CCheckDlg::CCheckDlg(std::vector<DWORD> vecCheckItemList, GLItemMan *pItemMan1, GLItemMan *pItemMan2 )
	: CDialog(CCheckDlg::IDD, NULL)
	, m_pBaseItemMan(pItemMan1)
	, m_pTargetItemMan(pItemMan2)
{
	for( int i = 0; i < vecCheckItemList.size(); i++ )
	{
		m_vecIDList.push_back(SIDLIST(vecCheckItemList[i]));
	}
}

CCheckDlg::~CCheckDlg()
{
}

void CCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_LIST, m_list_Check);
}


BEGIN_MESSAGE_MAP(CCheckDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_CHECK_LIST, OnNMDblclkCheckList)
	ON_BN_CLICKED(IDC_IDCHANGE_BUTTON, OnBnClickedIdchangeButton)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

	
// CCheckDlg 메시지 처리기입니다.

BOOL CCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	RECT listRect;
	m_list_Check.GetClientRect(&listRect);
	int nListWidth = ( listRect.right - listRect.left );
	int nColWidth[3] = { int(nListWidth*0.33f), int(nListWidth*0.33f), int(nListWidth*0.33f) };

	m_list_Check.SetExtendedStyle ( m_list_Check.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_list_Check.InsertColumn ( 0, _T("Name"),	LVCFMT_CENTER, nColWidth[0] );
	m_list_Check.InsertColumn ( 1, _T("현재 ID"),	LVCFMT_CENTER, nColWidth[1] );
	m_list_Check.InsertColumn ( 2, _T("적용될 ID"),	LVCFMT_CENTER, nColWidth[2] );

	UpdateIDList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.

}

void CCheckDlg::OnNMDblclkCheckList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	IDCheck();
	*pResult = 0;
}

void CCheckDlg::OnBnClickedIdchangeButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	IDCheck();
}

void CCheckDlg::UpdateIDList()
{
	m_list_Check.DeleteAllItems();

	char szTempChar[32];
	for( int i = 0; i < m_vecIDList.size(); i++ )
	{
		SITEM *pItem = m_pBaseItemMan->GetItem( m_vecIDList[i].sCheckID );
		if( !pItem ) continue;

		m_list_Check.InsertItem( i, pItem->GetName() );
		sprintf( szTempChar, "[%d/%d]", m_vecIDList[i].sCheckID.wMainID, m_vecIDList[i].sCheckID.wSubID );
		m_list_Check.SetItemText( i, 1, szTempChar );
		sprintf( szTempChar, "[%d/%d]", m_vecIDList[i].sApplyID.wMainID, m_vecIDList[i].sApplyID.wSubID );
		m_list_Check.SetItemText( i, 2, szTempChar );
	}
}

void CCheckDlg::IDCheck()
{
	POSITION pos = m_list_Check.GetFirstSelectedItemPosition();
	int nIndex = m_list_Check.GetNextSelectedItem(pos);
	if( nIndex == -1 )
	{
		MessageBox( "ID를 변경할 아이템을 선택하세요.", "알림" );
		return;
	}

	SITEM *pItem = m_pBaseItemMan->GetItem( m_vecIDList[nIndex].sCheckID );
	if( !pItem ) return;


	CIDCheckDlg idCheckDlg( m_vecIDList[nIndex].sCheckID, pItem->GetName(), m_pTargetItemMan );
	if( idCheckDlg.DoModal() == IDOK )
	{
		m_vecIDList[nIndex].sApplyID = idCheckDlg.m_sItemID;
		UpdateIDList();
	}
	
}

void CCheckDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool bMsg = FALSE;
	CString strTemp;
	for( int i = 0; i < m_vecIDList.size(); i++ )
	{	
		SITEM *pBaseItem   = m_pBaseItemMan->GetItem( m_vecIDList[i].sCheckID );
		SITEM *pTargetItem = m_pTargetItemMan->GetItem( m_vecIDList[i].sApplyID );
		if( pTargetItem != NULL )
		{
			bMsg = TRUE;
			CString str;
			str.Format( "%s -> %s\n", pBaseItem->GetName(), pTargetItem->GetName() );
			strTemp += str;
		}
	}
	CString strMsg = "아래와 같이 데이터가 바뀝니다. 이대로 하시겠습니까?\n\n";
	strMsg += strTemp;
	if( MessageBox( strMsg.GetString(), "알림", MB_OK ) == IDNO ) return;
	OnOK();
}
