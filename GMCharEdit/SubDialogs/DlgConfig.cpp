// DlgConfig.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgConfig.h"
#include "../Util/CGmCfg.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
// CDlgConfig 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgConfig, CDialog)
CDlgConfig::CDlgConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConfig::IDD, pParent)
	, m_strOdbcName(_T(""))
	, m_strUserName(_T(""))
	, m_strPassword(_T(""))
	, m_strPassword2(_T(""))
{
}

CDlgConfig::~CDlgConfig()
{
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, ID_EDIT_ODBC_NAME, m_strOdbcName);
	DDV_MaxChars(pDX, m_strOdbcName, 50);
	DDX_Text(pDX, ID_EDIT_USERNAME, m_strUserName);
	DDV_MaxChars(pDX, m_strUserName, 50);
	DDX_Text(pDX, ID_USER_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 50);
	DDX_Text(pDX, ID_USER_PASSWORD2, m_strPassword2);
	DDV_MaxChars(pDX, m_strPassword2, 50);
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgConfig 메시지 처리기입니다.

void CDlgConfig::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if (m_strPassword.Compare(m_strPassword2) == 0) // Zero if the strings are identical
	{
		CGmCfg::GetInstance()->SetOdbcName(m_strOdbcName);
		CGmCfg::GetInstance()->SetUserName(m_strUserName);
		CGmCfg::GetInstance()->SetUserPass(m_strPassword);
		CGmCfg::GetInstance()->SaveODBCInfo();
		OnOK();
	}
	else // Password is not identical
	{
		MessageBox(_T("Password is not identical"));
		return;
	}	
}
