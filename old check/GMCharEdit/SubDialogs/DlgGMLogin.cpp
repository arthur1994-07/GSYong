// DlgGMLogin.cpp : ���� �����Դϴ�.
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

// CDlgGMLogin ��ȭ �����Դϴ�.

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


// CDlgGMLogin �޽��� ó�����Դϴ�.

void CDlgGMLogin::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		//�ε��׽þƴ� VTC������ ����ϹǷ�, GMCharEdit�� ����ϱ� ���� UserInfo�� ���Ƿ� UserPass�� �����ؾ� �Ѵ�.
		//�� ��, ȥ���� ���̱� ���� �׳� �� �н����带 ����ϵ��� �ϰڴ�.
		//
		
		//m_strPassword;
	}
	*/
	
	m_nRetCode = m_pDbMan->GmLogin(m_strGMID, m_strPassword, csIP);
	
	//m_nRetCode = m_pDBMan->GmLogin(m_strGMID, m_strPassword, strUserIP);

	printf("[DEBUG] - #04\n");

	if (m_nRetCode < 20) // �α��� ����
	{
		CString str;
		str.Format("m_nRetCode(%d) %s %s %s",m_strGMID, m_strPassword, csIP);
		MessageBox(_T(str.GetString()));
		return;
	}
	else
	{
		OnOK();
	}
}