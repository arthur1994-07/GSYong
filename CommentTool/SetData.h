#pragma once

#include <string>
// SetData ��ȭ �����Դϴ�.

class SetDataDlg : public CDialog
{
	DECLARE_DYNAMIC(SetDataDlg)

public:
	std::string m_strData;

public:
	SetDataDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~SetDataDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SET_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
