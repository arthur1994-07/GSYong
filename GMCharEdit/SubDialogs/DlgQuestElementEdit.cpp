// DlgQuestElementEdit.cpp : ���� �����Դϴ�.
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

// CDlgQuestElementEdit ��ȭ �����Դϴ�.

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


// CDlgQuestElementEdit �޽��� ó�����Դϴ�.

BOOL CDlgQuestElementEdit::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    InitListHead();

    GLQUEST* pQuest = GLQuestMan::GetInstance().Find(m_pQuestProg->m_sNID.dwID);
    
    // pQuest->m_sNID.wMainID; // Quest ���� Main ID
    // pQuest->m_sNID.wSubID; // Quest ���� Sub ID
    // pQuest->m_strTITLE;    // Quest Title
    // pQuest->m_strCOMMENT; // Quest ����  

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

    // ���� ����Ʈ STEP
    DWORD dwStep = m_pQuestProg->m_dwSTEP;

    // �� ����Ʈ�� ��ü STEP
    DWORD dwTotalStep = pQuest->GetSTEPNUM();

    // 
    for (DWORD i=0; i<dwTotalStep; ++i)
    {
        GLQUEST_STEP* pStep = pQuest->GetSTEP(i);
        // ���� �ܰ迡 ���� ����
        if (i < dwStep) // �Ϸ�
        {
            SetData(pStep, _T("Completed"), i);
        }
        else if (i == dwStep) // ���� ������
        {
            SetData(pStep, _T("Progress"), i);
        }
        else if (i > dwStep) // ���� �������� ����
        {
            SetData(pStep, _T("--------"), i);
        }
    }

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgQuestElementEdit::SetData(GLQUEST_STEP* pStep, CString strStatus, const int nStepNumber)
{
    int nCount=0;	
    CString strTemp;
	nCount = m_ListQuestStep.GetItemCount();

    LV_ITEM lvItem;
	::memset(&lvItem, 0, sizeof(LV_ITEM));

	lvItem.mask = LVIF_TEXT; // �Ӽ� ����
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
	m_ListQuestStep.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
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