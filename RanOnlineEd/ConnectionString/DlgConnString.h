#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>


// DlgConnString 대화 상자입니다.

class DlgConnString : public CDialog
{
	DECLARE_DYNAMIC(DlgConnString)

public:
	DlgConnString(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgConnString();

	void ApplyToXML(int nType);
	void UpdateCurrentGroup(int nType);
	void ClearGroupUI();

	void Encrypt(std::string &strPlain, std::string &strEncrypted);
	void Encrypt(CString &csPlain, CString &csEncrypted);

	void Decrypt(std::string &strEncrypted, std::string &Plain);
	void Decrypt(CString &csEncrypted, CString &csPlain);

	void PrintMsg(CString &csMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CONNSTRINGS };

	enum 
	{ 
		DBTYPE_USERDB = 0,
		DBTYPE_GAMEDB = 1,
		DBTYPE_LOGDB  = 2,
		DBTYPE_MAX,
	};
	CString m_csDBTypeString[DBTYPE_MAX];	//to print the DB-type.


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	
	//=========== A GROUP ============
	//
	CString m_csType[DBTYPE_MAX];	
	CString m_csName[DBTYPE_MAX];	
	BYTE	m_byIP[DBTYPE_MAX][4];	
	CString m_csIP[DBTYPE_MAX];		// Byte-to-String
	CString m_csID[DBTYPE_MAX];
	CString m_csPW[DBTYPE_MAX];
	//
	//================================

	CString m_csCurrentGroup;		// XML-string for current working group.

	
	// vector includes xml of all groups only except current working group.
	//  xml of current working group is saved in m_csCurrentGroup.
	
	
	//====== each vector item includes XML-string for A GROUP ======
	//
	std::vector<CString> m_vGroups;	// XML-strings for all groups
	typedef std::vector<CString>::iterator	ITER_GROUPS;
	//
	//==============================================================


	int				m_nTypeHistory;

	
	CEdit			m_ebTitle;
	CEdit			m_ebName;
	CIPAddressCtrl	m_ipDB;
	CEdit			m_ebID;
	CEdit			m_ebPW;	
	CListBox		m_lbType;
	CEdit			m_ebXML;	
	CEdit			m_ebResult;
	

	afx_msg void OnLbnSelchangeListDbtype();
	afx_msg void OnBnClickedButtonApply();	
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonAddmore();
	afx_msg void OnBnClickedButtonExport();
};
