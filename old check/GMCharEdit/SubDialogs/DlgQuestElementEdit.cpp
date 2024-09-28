// DlgQuestElementEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgQuestElementEdit.h"

#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgQuestElementEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgQuestElementEdit, CDialog)
CDlgQuestElementEdit::CDlgQuestElementEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQuestElementEdit::IDD, pParent)
    , m_strQuestName(_T(""))
    , m_strQuestComment(_T(""))
{
}

CDlgQuestElementEdit::~CDlgQuestElementEdit()
{
}

void CDlgQuestElementEdit::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_QUEST_NAME, m_strQuestName);
    DDX_Text(pDX, IDC_EDIT_QUEST_COMMENT, m_strQuestComment);
    DDX_Control(pDX, IDC_LIST1, m_ListQuestStep);
}


BEGIN_MESSAGE_MAP(CDlgQuestElementEdit, CDialog)
END_MESSAGE_MAP()


// CDlgQuestElementEdit 메시지 처리기입니다.

BOOL CDlgQuestElementEdit::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    InitListHead();

    GLQUEST* pQuest = GLQuestMan::GetInstance().Find(m_pQuestProg->m_sNID.dwID);
    
    // pQuest->m_sNID.wMainID; // Quest 고유 Main ID
    // pQuest->m_sNID.wSubID; // Quest 고유 Sub ID
    // pQuest->m_strTITLE;    // Quest Title
    // pQuest->m_strCOMMENT; // Quest 설명  

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strTitleKey, strCommentKey;
		strTitleKey.Format(_T("QN_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		strCommentKey.Format(_T("QD_%03d_%03d"), pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		m_strQuestName = CGameTextMan::GetInstance().GetQuestText(strTitleKey.GetString());
		m_strQuestComment = CGameTextMan::GetInstance().GetQuestText(strCommentKey.GetString());
	}
	else
	{
		m_strQuestName    = pQuest->m_strTITLE.c_str();
		m_strQuestComment = pQuest->m_strCOMMENT.c_str();
	}

    // 현재 퀘스트 STEP
    DWORD dwStep = m_pQuestProg->m_dwSTEP;

    // 이 퀘스트의 전체 STEP
    DWORD dwTotalStep = pQuest->GetSTEPNUM();

    // 
    for (DWORD i=0; i<dwTotalStep; ++i)
    {
        GLQUEST_STEP* pStep = pQuest->GetSTEP(i);
        // 현재 단계에 대한 제목
        if (i < dwStep) // 완료
        {
            SetData(pStep, _T("Completed"), i);
        }
        else if (i == dwStep) // 현재 진행중
        {
            SetData(pStep, _T("Progress"), i);
        }
        else if (i > dwStep) // 아직 시작하지 않음
        {
            SetData(pStep, _T("--------"), i);
        }
    }

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgQuestElementEdit::SetData(GLQUEST_STEP* pStep, CString strStatus, const int nStepNumber)
{
    int nCount=0;	
    CString strTemp;
	nCount = m_ListQuestStep.GetItemCount();

    LV_ITEM lvItem;
	::memset(&lvItem, 0, sizeof(LV_ITEM));

	lvItem.mask = LVIF_TEXT; // 속성 지정
	lvItem.iItem = nCount;
	lvItem.iSubItem = 0;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		strKey.Format("QN_%03d_%03d_P_%d", m_pQuestProg->m_sNID.wMainID, m_pQuestProg->m_sNID.wSubID, nStepNumber);
		strTemp = CGameTextMan::GetInstance().GetQuestText(strKey.GetString());
	}
	else
	{
		strTemp = pStep->GetTITLE();
	}
	lvItem.pszText = strTemp.GetBuffer();
	m_ListQuestStep.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
	m_ListQuestStep.SetItemText(nCount, 1, strStatus);

}

void CDlgQuestElementEdit::InitListHead()
{
    LVCOLUMN Col;
	CString strTemp;
	
	strTemp			= "Step Title";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuestStep.InsertColumn(0, &Col);

	strTemp			= "Status";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 100;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListQuestStep.InsertColumn(1, &Col);	
}