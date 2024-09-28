// DlgSkillElementEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgSkillElementEdit.h"

#include "../MainFrm.h"
#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgSkillElementEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSkillElementEdit, CDialog)
CDlgSkillElementEdit::CDlgSkillElementEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSkillElementEdit::IDD, pParent)
    , m_strMainID(_T(""))
    , m_strSubID(_T(""))
    , m_strSkillName(_T(""))
    , m_strLevel(_T(""))
{
}

CDlgSkillElementEdit::~CDlgSkillElementEdit()
{
}

void CDlgSkillElementEdit::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT23, m_strMainID);
    DDX_Text(pDX, IDC_EDIT31, m_strSubID);
    DDX_Text(pDX, IDC_EDIT1, m_strSkillName);
    DDX_Text(pDX, IDC_EDIT2, m_strLevel);
}


BEGIN_MESSAGE_MAP(CDlgSkillElementEdit, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSkillElementEdit 메시지 처리기입니다.

void CDlgSkillElementEdit::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    UpdateData(TRUE);
    OnOK();
}
