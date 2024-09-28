#pragma once
#include "afxwin.h"


// CSetVar 대화 상자입니다.

#define USECHECK			(0x0001)
#define USERADIO			(0x0002)
#define USEDOWN				(0x0004)
#define USESTARTSCROLL		(0x0010)
#define USESTARTVARSCROLL	(0x0020)
#define USEENDSCROLL		(0x0040)
#define USEEND4SCROLL		(0x0080)

class CSetVar : public CDialog
{
	DECLARE_DYNAMIC(CSetVar)

public:
	DWORD	m_dwFlag;
	BOOL	m_bUse;
	int		m_iRadio;
	float	m_fStart;
	float	m_fStartVar;
	float	m_fEnd;
	float	m_fEnd4;

public:
	float	m_ToScale;
	int		m_Zero;
	BOOL	m_bZeroBase;

public:
	CString	m_StrTitle;

	CString m_StrUse;
	CString m_StrRadio1;
	CString m_StrRadio2;
	CString m_StrComm1;
	CString m_StrComm2;
	CString m_StrComm3;
	CString m_StrComm4;

public:
	CSetVar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetVar();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SETVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CScrollBar m_ScrollStart;
	CScrollBar m_ScrollVar;
	CScrollBar m_ScrollEnd;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditStartF();
	afx_msg void OnEnChangeEditStartvarF();
	afx_msg void OnEnChangeEditEndF();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckUse();
	CScrollBar m_ScrollEnd4;
	afx_msg void OnEnChangeEditEnd4F();
	afx_msg void OnBnClickedRadioUp();
	afx_msg void OnBnClickedRadioMid();
	afx_msg void OnBnClickedRadioDown();
};
