// DlgSkillElementEdit.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgSkillElementEdit.h"

#include "../MainFrm.h"
#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgSkillElementEdit ��ȭ �����Դϴ�.

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


// CDlgSkillElementEdit �޽��� ó�����Դϴ�.

void CDlgSkillElementEdit::OnBnClickedOk()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

    UpdateData(TRUE);
    OnOK();
}
