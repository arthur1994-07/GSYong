#pragma once

#include <string>
// SetData 대화 상자입니다.

class SetDataDlg : public CDialog
{
	DECLARE_DYNAMIC(SetDataDlg)

public:
	std::string m_strData;

public:
	SetDataDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~SetDataDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SET_DATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
