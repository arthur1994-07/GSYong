/**
 * \date	2011/01/25
 * \author	Jun-Hyuk Choi
 */
#pragma once

class CInputDlg : public CDialog
{
protected:
	DECLARE_DYNAMIC( CInputDlg )

public:
    CString m_strTitle;
    CString m_strComment;
    CString m_strValue;

public:
	CInputDlg( CWnd* pParent = NULL );
    virtual ~CInputDlg() {}

protected:
	virtual VOID DoDataExchange( CDataExchange* pDX );
    virtual BOOL OnInitDialog();
};