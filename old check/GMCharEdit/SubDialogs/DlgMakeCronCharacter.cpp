// DlgMakeCronCharacter.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgMakeCronCharacter.h"
#include "../MainFrm.h"

#include "../../RanLogicServer/DxServerInstance.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgMakeCronCharacter ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgMakeCronCharacter, CDialog)
CDlgMakeCronCharacter::CDlgMakeCronCharacter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMakeCronCharacter::IDD, pParent)
	, m_nUserNum(0)
	, m_strChaName(_T(""))
{
}

CDlgMakeCronCharacter::~CDlgMakeCronCharacter()
{
}

void CDlgMakeCronCharacter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAKE_CRON_NUM, m_nUserNum);
	DDX_Text(pDX, IDC_EDIT_MAKE_CRON_NAME, m_strChaName);
}


BEGIN_MESSAGE_MAP(CDlgMakeCronCharacter, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMakeCronCharacter �޽��� ó�����Դϴ�.

void CDlgMakeCronCharacter::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_MASTER)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}	

	UpdateData(TRUE);

	if ( m_strChaName.GetLength() > 16 )
	{
		MessageBox(_T("ChaName Length Error"));
		return;
	}


	OnOK();
}
