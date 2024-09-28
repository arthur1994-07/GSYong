// ConnectionString\DlgConnString.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <string>
#include "../SigmaCore/Encrypt/DbEncrypt.h"
//#include "../SigmaCore/String/StringUtil.h"
#include "../RanOnlineEd.h"
#include "./DlgConnString.h"


// DlgConnString 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgConnString, CDialog)

DlgConnString::DlgConnString(CWnd* pParent /*=NULL*/)
	: CDialog(DlgConnString::IDD, pParent)
	,m_nTypeHistory(-1)
{
	m_csDBTypeString[DBTYPE_USERDB] = _T("UserDB");
	m_csDBTypeString[DBTYPE_GAMEDB] = _T("GameDB");
	m_csDBTypeString[DBTYPE_LOGDB] = _T("LogDB");
}

DlgConnString::~DlgConnString()
{
}

void DlgConnString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_XML, m_ebXML);
	DDX_Control(pDX, IDC_IPADDRESS_DB, m_ipDB);	
	DDX_Control(pDX, IDC_EDIT_DBNAME, m_ebName);
	DDX_Control(pDX, IDC_EDIT_DBID, m_ebID);
	DDX_Control(pDX, IDC_EDIT_DBPW, m_ebPW);
	DDX_Control(pDX, IDC_LIST_DBTYPE, m_lbType);
	DDX_Control(pDX, IDC_EDIT_RESULT, m_ebResult);
	DDX_Control(pDX, IDC_EDIT_TITLE, m_ebTitle);
}


BEGIN_MESSAGE_MAP(DlgConnString, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_DBTYPE, &DlgConnString::OnLbnSelchangeListDbtype)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &DlgConnString::OnBnClickedButtonApply)	
	ON_BN_CLICKED(IDC_BUTTON_RESET, &DlgConnString::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_ADDMORE, &DlgConnString::OnBnClickedButtonAddmore)		
	ON_BN_CLICKED(ID_BUTTON_EXPORT, &DlgConnString::OnBnClickedButtonExport)
END_MESSAGE_MAP()


// DlgConnString 메시지 처리기입니다.

BOOL DlgConnString::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	ZeroMemory(m_byIP, sizeof(m_byIP));

	

	m_lbType.InsertString(DBTYPE_USERDB, _T("UserDB"));
	m_lbType.InsertString(DBTYPE_GAMEDB, _T("GmaeDB"));
	m_lbType.InsertString(DBTYPE_LOGDB, _T("LogDB"));

	m_lbType.SetCurSel(DBTYPE_USERDB);	//default
	OnLbnSelchangeListDbtype();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgConnString::OnLbnSelchangeListDbtype()
{
	//
	//save input information
	//

	UpdateCurrentGroup(m_nTypeHistory);
	
	int nType = m_lbType.GetCurSel();

	m_nTypeHistory = nType;
	
	m_ebName.SetWindowText(m_csName[nType]);
	m_ebID.SetWindowText(m_csID[nType]);
	m_ebPW.SetWindowText(m_csPW[nType]);
	m_ipDB.SetAddress(m_byIP[nType][0], m_byIP[nType][1], m_byIP[nType][2], m_byIP[nType][3]);

	CString csMsg;

	csMsg.Format(_T("selected %s"), m_csDBTypeString[nType].GetBuffer());

	PrintMsg(CString(_T("Export Done.")));
}


void DlgConnString::OnBnClickedButtonApply()
{
	int nType = m_lbType.GetCurSel();	

	UpdateCurrentGroup(nType);

	ApplyToXML(nType);

	PrintMsg(CString(_T("Apply Done.")));
}


void DlgConnString::UpdateCurrentGroup(int nType)
{
	//
	// -1 is the m_nHistory's initialized value that means there's not valid informations on the UIs.
	//  just ignore this call.
	//
	if (nType == -1)
		return;
	
	CString csTitle;
	m_ebTitle.GetWindowText(csTitle);

	m_ebName.GetWindowText(m_csName[nType]);
	m_ebID.GetWindowText(m_csID[nType]);
	m_ebPW.GetWindowText(m_csPW[nType]);
	
	m_ipDB.GetAddress( m_byIP[nType][0], m_byIP[nType][1], m_byIP[nType][2], m_byIP[nType][3]);
	m_csIP[nType].Format(_T("%d.%d.%d.%d"), (int)m_byIP[nType][0], (int)m_byIP[nType][1], (int)m_byIP[nType][2], (int)m_byIP[nType][3]);


	//
	// Encrypt for creating XML output
	//
	for (int i=0; i<DBTYPE_MAX; i++)
	{
		Encrypt(m_csIP[i],	m_csIP[i]);
		Encrypt(m_csName[i],m_csName[i]);
		Encrypt(m_csID[i],	m_csID[i]);
		Encrypt(m_csPW[i],	m_csPW[i]);
	}

	//
	// apply to xml of current working group
	//
	m_csCurrentGroup.Format(_T(
					"	<group>\r\n"				\
					"		<title>\r\n"			\
					"		%s\r\n"					\
					"		</title>\r\n"			\
					"\r\n"							\
					"		<userdb>\r\n"			\
					"			<name>%s</name>\r\n"\
					"			<ip>%s</ip>\r\n"	\
					"			<id>%s</id>\r\n"	\
					"			<pw>%s</pw>\r\n"	\
					"		</userdb>\r\n"			\
					"\r\n"							\
					"		<gamedb>\r\n"			\
					"			<name>%s</name>\r\n"\
					"			<ip>%s</ip>\r\n"	\
					"			<id>%s</id>\r\n"	\
					"			<pw>%s</pw>\r\n"	\
					"		</gamedb>\r\n"			\
					"\r\n"							\
					"		<logdb>\r\n"			\
					"			<name>%s</name>\r\n"\
					"			<ip>%s</ip>\r\n"	\
					"			<id>%s</id>\r\n"	\
					"			<pw>%s</pw>\r\n"	\
					"		</logdb>\r\n"			\
					"	</group>\r\n"				\
					),csTitle
					,m_csName[DBTYPE_USERDB].GetBuffer(), m_csIP[DBTYPE_USERDB].GetBuffer(), m_csID[DBTYPE_USERDB].GetBuffer(), m_csPW[DBTYPE_USERDB].GetBuffer()
					,m_csName[DBTYPE_GAMEDB].GetBuffer(), m_csIP[DBTYPE_GAMEDB].GetBuffer(), m_csID[DBTYPE_GAMEDB].GetBuffer(), m_csPW[DBTYPE_GAMEDB].GetBuffer()
					,m_csName[DBTYPE_LOGDB].GetBuffer(), m_csIP[DBTYPE_LOGDB].GetBuffer(), m_csID[DBTYPE_LOGDB].GetBuffer(), m_csPW[DBTYPE_LOGDB].GetBuffer()					
					);

	
	//
	// 단순한 Tool이므로 성능보다는 편의성 위주로 작성하도록 한다.
	//

	//
	//Decrypt for UI-output
	//
	for (int i=0; i<DBTYPE_MAX; i++)
	{
		Decrypt(m_csIP[i],	m_csIP[i]);
		Decrypt(m_csName[i],m_csName[i]);
		Decrypt(m_csID[i],	m_csID[i]);
		Decrypt(m_csPW[i],	m_csPW[i]);
	}
}



void DlgConnString::ApplyToXML(int nType)
{
	CString csXML;
	CString csGroups;

	if ( !m_vGroups.empty() )
	{
		ITER_GROUPS iter;

		for (iter = m_vGroups.begin(); iter != m_vGroups.end(); iter++)
		{
			csGroups.Append(*iter);
		}
	}

	csXML.Format(_T(
			"<?xml version=\"1.0\"?>\r\n"	\
			"<groups>\r\n"				\
			"%s"							\
			"%s"							\
			"</groups>\r\n"
		), csGroups.GetBuffer(), m_csCurrentGroup.GetBuffer() );



	m_ebXML.SetWindowText(csXML);
}


void DlgConnString::ClearGroupUI()
{
	//
	// empty all UIs and variables related to the UIs.
	//
	m_ebTitle.SetWindowText(_T(""));
	m_ebName.SetWindowText(_T(""));
	m_ipDB.SetAddress(0,0,0,0);
	m_ebID.SetWindowText(_T(""));
	m_ebPW.SetWindowText(_T(""));

	
	m_csCurrentGroup.Empty();

	
	for(int i = 0; i<DBTYPE_MAX; i++)
	{
		m_csType[i].Empty();
		m_csName[i].Empty();
		m_csID[i].Empty();
		m_csPW[i].Empty();
		m_csIP[i].Empty();
	}
	
	ZeroMemory(m_byIP, sizeof(m_byIP));
}


void DlgConnString::OnBnClickedButtonAddmore()
{
	if (m_csCurrentGroup.IsEmpty())
	{
		PrintMsg(CString(_T("Current Group-info is empty. Apply first.")));
		
		return;
	}	

	//
	// insert current working group into the group's vector.
	//
	m_vGroups.push_back(m_csCurrentGroup);	

	ClearGroupUI();

	m_lbType.SetCurSel(DBTYPE_USERDB);
	OnLbnSelchangeListDbtype();

	PrintMsg(CString(_T("Added a server-group.")));
}


void DlgConnString::OnBnClickedButtonReset()
{
	ClearGroupUI();

	m_ebXML.SetWindowText(_T(""));

	//
	// erase all groups.
	//
	m_vGroups.clear();

	m_lbType.SetCurSel(DBTYPE_USERDB);
	OnLbnSelchangeListDbtype();

	PrintMsg(CString(_T("Reset all contents.")));
}


void DlgConnString::Encrypt(std::string &strPlain, std::string &strEncrypted)
{
    sc::DbEncrypt Encrypt;
    Encrypt.encrypt(strPlain, strEncrypted);
}


void DlgConnString::Encrypt(CString &csPlain, CString &csEncrypted)
{
	std::string strEncrypted;

	Encrypt(std::string(csPlain.GetBuffer()), strEncrypted);

	csEncrypted.SetString(strEncrypted.c_str());
}


void DlgConnString::Decrypt(std::string &strEncrypted, std::string &strPlain)
{	
    sc::DbEncrypt Decrypt;
    Decrypt.decrypt(strEncrypted, strPlain);
}


void DlgConnString::Decrypt(CString &csEncrypted, CString &csPlain)
{
	std::string strPlain;

	Decrypt(std::string(csEncrypted.GetBuffer()), strPlain);

	csPlain.SetString(strPlain.c_str());
}

void DlgConnString::OnBnClickedButtonExport()
{
	CString csXML;

	m_ebXML.GetWindowText(csXML);

	csXML.Trim(_T('\r'));	//remove all carage-return.    

	

	CTime	ctCurrent;	
	ctCurrent = CTime::GetCurrentTime();
	
	CString csFileName;	
	csFileName.Format(_T(".\\GMCharEdit-%s.xml"), ctCurrent.Format(_T("%y%m%d_%H%M%S")) );
	
	CFile cfExport;
	
	
	if ( !cfExport.Open(csFileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate) )
	{
		PrintMsg(CString(_T("Could not create a Export-File.")));
		
		return;
	}	

	cfExport.Write(csXML.GetBuffer(), csXML.GetLength());

	cfExport.Close();	

	PrintMsg(CString(_T("Export Done.")));
}

void DlgConnString::PrintMsg(CString &csMsg)
{
	m_ebResult.SetWindowText(csMsg);
}
