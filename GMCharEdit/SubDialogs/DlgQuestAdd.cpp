// DlgQuestAdd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgQuestAdd.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"

#include "../MainFrm.h"
#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgQuestAdd 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgQuestAdd, CDialog)
CDlgQuestAdd::CDlgQuestAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQuestAdd::IDD, pParent)
{
    m_nSelected = -1;
}

CDlgQuestAdd::~CDlgQuestAdd()
{
}

void CDlgQuestAdd::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_QUEST_ADD, m_ListQuestAdd);
}


BEGIN_MESSAGE_MAP(CDlgQuestAdd, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)    
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_QUEST_ADD, OnNMDblclkListQuestAdd)
END_MESSAGE_MAP()


// CDlgQuestAdd 메시지 처리기입니다.

BOOL CDlgQuestAdd::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_ListQuestAdd.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    InitListHead();
    SetData();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgQuestAdd::InitListHead()
{
    LVCOLUMN Col;
	CString strTemp;
	// Num, Name, Class, Tribe, Level,
	strTemp			= "Main";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuestAdd.InsertColumn(0, &Col);

	strTemp			= "Sub";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuestAdd.InsertColumn(1, &Col);

	strTemp			= "Title";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuestAdd.InsertColumn(2, &Col);  

    strTemp			= "Desc";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 300;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuestAdd.InsertColumn(3, &Col);
}

void CDlgQuestAdd::SetData()
{
    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

    // Erase all list items
	m_ListQuestAdd.DeleteAllItems();    

    m_vQuest.erase(m_vQuest.begin(), m_vQuest.end());    

    GLQuestMan::MAPQUEST pQuestMap = GLQuestMan::GetInstance().GetMap();
    GLQuestMan::MAPQUEST_ITER iterStart = pQuestMap.begin();
    GLQuestMan::MAPQUEST_ITER iterEnd   = pQuestMap.end();        
		
    int nNum=0;

	for ( ; iterStart!=iterEnd; ++iterStart)
	{
        const GLQuestMan::SNODE &sNode = (*iterStart).second;
        GLQUEST* pQuest = sNode.pQUEST;

		if ( !pQuest )		continue;

        pQuest->m_sNID.wMainID; // Quest 고유 Main ID
        pQuest->m_sNID.wSubID; // Quest 고유 Sub ID
        pQuest->m_strTITLE;    // Quest Title
        pQuest->m_strCOMMENT; // Quest 설명
            
        int nCount=0;
		CString strTemp;
		nCount = m_ListQuestAdd.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));

		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
        strTemp.Format("%d", pQuest->m_sNID.dwID);
		lvItem.pszText = strTemp.GetBuffer();		
		m_ListQuestAdd.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
    			
        strTemp.Format("%d", pQuest->m_sNID.wSubID);
		m_ListQuestAdd.SetItemText(nNum, 1, strTemp);
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strTitleKey, strCommentKey;
			strTitleKey.Format(_T("QN_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
			strCommentKey.Format(_T("QD_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
			m_ListQuestAdd.SetItemText(nNum, 2, CGameTextMan::GetInstance().GetQuestText(strTitleKey.GetString()));
			m_ListQuestAdd.SetItemText(nNum, 3, CGameTextMan::GetInstance().GetQuestText(strCommentKey.GetString()));
		}
		else
		{
			m_ListQuestAdd.SetItemText(nNum, 2, pQuest->m_strTITLE.c_str());
			m_ListQuestAdd.SetItemText(nNum, 3, pQuest->m_strCOMMENT.c_str());
		}

        GLQUEST sQuest;
        sQuest.m_sNID.wMainID = pQuest->m_sNID.wMainID;
        sQuest.m_sNID.wSubID  = pQuest->m_sNID.wSubID;
        
        m_vQuest.push_back(sQuest);

        nNum++;
    }
}

void CDlgQuestAdd::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    UINT uSelectedCount = m_ListQuestAdd.GetSelectedCount();
    int  nItem = -1;

    // Check selected item
    if (uSelectedCount > 0)
    {
        for (UINT i=0;i < uSelectedCount;i++)
        {
	        nItem = m_ListQuestAdd.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
        		
	        if (nItem != -1) // 선택된 항목이 있음
	        {	
                m_vSelected.push_back(nItem);
                m_nSelected = 1;
            }
            else
            {
                nItem = -1;
            }
        }
    }

    OnOK();
}

void CDlgQuestAdd::OnNMDblclkListQuestAdd(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    OnBnClickedOk();

    *pResult = 0;
}
