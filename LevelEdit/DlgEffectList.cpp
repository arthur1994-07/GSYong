// DlgEffectList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LevelEdit.h"
#include "DlgEffectList.h"

#include "LevelMainFrm.h"
#include "LevelEditDoc.h"
#include "LevelEditView.h"

// CDlgEffectList 대화 상자입니다.

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


// CDlgEffectList 메시지 처리기입니다.

BOOL CDlgEffectList::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
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
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
