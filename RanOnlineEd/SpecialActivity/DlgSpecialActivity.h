#pragma once

#include "../Resource.h"

class CsheetWithTab;

// DlgSpecialActivity ��ȭ �����Դϴ�.

class DlgSpecialActivity : public CDialog
{
	DECLARE_DYNAMIC(DlgSpecialActivity)

public:
	DlgSpecialActivity(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgSpecialActivity();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLGSPECIALACTIVITY };

public:
    CsheetWithTab	*m_pSheetTab;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonNew();
    afx_msg void OnBnClickedButtonLoad();
    afx_msg void OnBnClickedButtonSave();
    afx_msg void OnBnClickedButtonXmlExport();
    afx_msg void OnBnClickedButtonXmlImport();
    afx_msg void OnBnClickedButtonCsvExport();
};
