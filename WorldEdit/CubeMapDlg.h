#pragma once
#include "MainFrm.h"
#include "afxcmn.h"


// CCubeMapDlg 대화 상자입니다.

class CCubeMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CCubeMapDlg)

public:
	CCubeMapDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCubeMapDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CUBEMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMap0();
	afx_msg void OnBnClickedButtonMap1();
	afx_msg void OnBnClickedButtonMap2();
	afx_msg void OnBnClickedButtonMap3();
	afx_msg void OnBnClickedButtonMap4();
	afx_msg void OnBnClickedButtonMap5();
	CString m_strMap0;
	CString m_strMap1;
	CString m_strMap2;
	CString m_strMap3;
	CString m_strMap4;
	CString m_strMap5;
	float m_fAlpha;

	//멤버 변수;
	CString m_strMap00;
	CString m_strMap11;
	CString m_strMap22;
	CString m_strMap33;
	CString m_strMap44;
	CString m_strMap55;
	float m_fAlpha0;

	int m_nSpinPos;
	

	void SetInit(char* strMap0, char* strMap1, char* strMap2, char* strMap3, char* strMap4, char* strMap5, float fAlpha);
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_ctrlReflectionAlpha;
	afx_msg void OnDeltaposSpinAlpha(NMHDR *pNMHDR, LRESULT *pResult);
};
