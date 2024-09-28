#pragma once


// CCollisionMapDlg 대화 상자입니다.

class CCollisionMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CCollisionMapDlg)

public:
	CCollisionMapDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCollisionMapDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_COLLISIONMAP };

private:
	HANDLE	m_hThread;
	DWORD	m_ThreadID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
};
