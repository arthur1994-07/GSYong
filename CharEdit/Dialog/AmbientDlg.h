#pragma once


// CAmbientDlg ��ȭ �����Դϴ�.

class CAmbientDlg : public CDialog
{
	DECLARE_DYNAMIC(CAmbientDlg)

public:
    BOOL    m_bAmbientTest;
	DWORD	m_dwAmbient;

public:
	CAmbientDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAmbientDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_AMBIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
};
