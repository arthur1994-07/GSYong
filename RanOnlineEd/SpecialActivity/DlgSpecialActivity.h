#pragma once

#include "../Resource.h"

class CsheetWithTab;

// DlgSpecialActivity 대화 상자입니다.

class DlgSpecialActivity : public CDialog
{
	DECLARE_DYNAMIC(DlgSpecialActivity)

public:
	DlgSpecialActivity(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgSpecialActivity();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLGSPECIALACTIVITY };

public:
    CsheetWithTab	*m_pSheetTab;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
