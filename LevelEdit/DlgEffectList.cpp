// DlgEffectList.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LevelEdit.h"
#include "DlgEffectList.h"

#include "LevelMainFrm.h"
#include "LevelEditDoc.h"
#include "LevelEditView.h"

// CDlgEffectList ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgEffectList, CDialog)

CDlgEffectList::CDlgEffectList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEffectList::IDD, pParent)
    , m_pSelEff(0)
{

}

CDlgEffectList::~CDlgEffectList()
{
}

void CDlgEffectList::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_kEffListBox);
}


BEGIN_MESSAGE_MAP(CDlgEffectList, CDialog)
    ON_BN_CLICKED(IDOK, &CDlgEffectList::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgEffectList �޽��� ó�����Դϴ�.

BOOL CDlgEffectList::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
    CLevelEditView* pView = (CLevelEditView *) pFrame->GetActiveView ();    
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();
    GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();

    if (pGLLandMan)
    {
        PLANDEFF pCur = pGLLandMan->GetLandEffList( 0 );

        while (pCur)
        {        
            int nIndex = m_kEffListBox.AddString(pCur->m_szName);
            m_kEffListBox.SetItemData(nIndex, (DWORD)pCur);
            pCur = pCur->m_pNext;
        }
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

DXLANDEFF* CDlgEffectList::GetSelEffect()
{
    return m_pSelEff;
}

void CDlgEffectList::OnBnClickedOk()
{
    int nIndex = m_kEffListBox.GetCurSel();

    if (nIndex != LB_ERR)
    {
        m_pSelEff = (DXLANDEFF*)m_kEffListBox.GetItemData(nIndex);
    }

    OnOK();
}
