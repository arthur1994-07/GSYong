#pragma once


// CAmbientDlg 대화 상자입니다.

class CAmbientDlg : public CDialog
{
	DECLARE_DYNAMIC(CAmbientDlg)

public:
    BOOL    m_bAmbientTest;
	DWORD	m_dwAmbient;

public:
	CAmbientDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAmbientDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_AMBIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
};
