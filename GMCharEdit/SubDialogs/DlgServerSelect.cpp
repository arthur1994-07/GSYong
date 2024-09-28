// DlgServerSelect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgServerSelect.h"

#include "../MainFrm.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../Util/CGmCfg.h"
#include "../../SigmaCore/Util/SystemInfo.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgServerSelect 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgServerSelect, CDialog)

CDlgServerSelect::CDlgServerSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgServerSelect::IDD, pParent)
{
	m_nSelectedServer = -1;
}

CDlgServerSelect::~CDlgServerSelect()
{
}

void CDlgServerSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER_SELECT, m_ListServerSelect);
}


BEGIN_MESSAGE_MAP(CDlgServerSelect, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgServerSelect 메시지 처리기입니다.

void CDlgServerSelect::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelected = m_ListServerSelect.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected == -1) // 선택된 서버가 없음
	{
		MessageBox(_T("Please Select Server"));
	}
	else
	{		
		m_nSelectedServer = nSelected;

		OnOK();
	}
}

BOOL CDlgServerSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
    CString strCaption;
    strCaption.Format( _T(" Ver %s"), sc::getFileVersion().c_str() );
    SetWindowText( strCaption.GetBuffer() );

	m_ListServerSelect.SetExtendedStyle (LVS_EX_FULLROWSELECT);
	InitListHead();

	FillData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 리스트 컨트롤 헤더 초기화
void CDlgServerSelect::InitListHead()
{
	LVCOLUMN Col;
	CString strTemp;	
	strTemp			= "Select a Server Group";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 1000;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListServerSelect.InsertColumn(0, &Col);
}


void CDlgServerSelect::FillData()
{
	// Erase all list items
	m_ListServerSelect.DeleteAllItems();

	//typedef std::vector<gmce::A_GROUP>::iterator	iterGROUPS;

	if (CGmCfg::GetInstance()->m_nDBType == gmce::ADO)	//ADO
	{
		CGmCfg *p = CGmCfg::GetInstance();
		
		for (gmce::iterGROUPS iter = p->m_vGROUPS.begin(); iter != p->m_vGROUPS.end(); ++iter)
		{
			int nCount; 		
			nCount = m_ListServerSelect.GetItemCount();

			CString csTmp;

			csTmp = CString(iter->title.c_str());

			LV_ITEM lvItem;
			::memset(&lvItem, 0, sizeof(LV_ITEM));

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nCount;
			lvItem.iSubItem = 0;        
			lvItem.pszText = csTmp.GetBuffer();
			m_ListServerSelect.InsertItem(&lvItem);
		}


		/*
		int nSGNum = CGmCfg::GetInstance()->m_dwSGNum;

		for (int i=0; i < nSGNum; i++)
		{
			int nCount; 		
			nCount = m_ListServerSelect.GetItemCount();

			CString csTmp;

			csTmp.Format("Server-Group #%d", i);

			LV_ITEM lvItem;
			::memset(&lvItem, 0, sizeof(LV_ITEM));

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nCount;
			lvItem.iSubItem = 0;        
			lvItem.pszText = csTmp.GetBuffer();
			m_ListServerSelect.InsertItem(&lvItem);
		}
		*/
	}
	else												//ODBC
	{
		m_vServerList.clear();
		
		if (CGMOdbcManager::GetInstance()->GetServerInfo(m_vServerList) == DB_ERROR)
		{
			return;
		}
		
		for (int i=0; i< (int) m_vServerList.size(); ++i)
		{		
			// Num, Name, Class, Tribe, Level,
			int nCount; 
			CString strTemp;
			nCount = m_ListServerSelect.GetItemCount();

			LV_ITEM lvItem;
			::memset(&lvItem, 0, sizeof(LV_ITEM));

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nCount;
			lvItem.iSubItem = 0;        
			lvItem.pszText = m_vServerList[i].strName.GetBuffer();
			m_ListServerSelect.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
		}
	}
}
