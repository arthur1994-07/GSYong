// DlgGMLogin.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgGMLogin.h"

#include "../Util/CGmCfg.h"

#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "../../SigmaCore/String/StringUtil.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgGMLogin 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgGMLogin, CDialog)
CDlgGMLogin::CDlgGMLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGMLogin::IDD, pParent)
	, m_strGMID(_T(""))
	, m_strPassword(_T(""))	
{
	//mjeon
	m_pDbMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CDlgGMLogin::~CDlgGMLogin()
{
}

void CDlgGMLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GMID, m_strGMID);
	DDV_MaxChars(pDX, m_strGMID, 20);
	DDX_Text(pDX, IDC_EDIT_GMPASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 20);
}


BEGIN_MESSAGE_MAP(CDlgGMLogin, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgGMLogin 메시지 처리기입니다.

void CDlgGMLogin::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	printf("[DEBUG] - #01\n");

	m_pDbMan->SetLocalIP();
	//m_pDBMan->SetLocalIP();

	printf("[DEBUG] - #02\n");
	
	CString csIP;
	
	csIP = m_pDbMan->GetLocalIP();
	//csIP = m_pDBMan->GetLocalIP();

	printf("[DEBUG] - #03\n");

	/*
	if (SP_INDONESIA == GLogicData::GetInstance().GetServiceProvider())
	{
		//
		//인도네시아는 VTC인증을 사용하므로, GMCharEdit를 사용하기 위해 UserInfo에 임의로 UserPass를 설정해야 한다.
		//이 때, 혼란을 줄이기 위해 그냥 평문 패스워드를 사용하도록 하겠다.
		//
		
		//m_strPassword;
	}
	*/
	
	m_nRetCode = m_pDbMan->GmLogin(m_strGMID, m_strPassword, csIP);
	
	//m_nRetCode = m_pDBMan->GmLogin(m_strGMID, m_strPassword, strUserIP);

	printf("[DEBUG] - #04\n");

	if (m_nRetCode < 20) // 로그인 실패
	{
		MessageBox(_T("Login Failed"));
		return;
	}
	else
	{
		OnOK();
	}
}