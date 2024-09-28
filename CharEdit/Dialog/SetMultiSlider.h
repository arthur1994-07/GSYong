#pragma once
#include "afxcmn.h"

#include "../MfcExLib/ExLibs/multislider.h"

// CSetMultiSlider 대화 상자입니다.

#define USECHECK	(0x0001)
#define USEEDIT		(0x0002)

class CSetMultiSlider : public CDialog
{
	DECLARE_DYNAMIC(CSetMultiSlider)

public:
	COLORREF m_colors;

	DWORD	m_dwFlag;

	BOOL	m_bUse;

	float	m_fTemp;

	float	m_fStart;
	float	m_fMid1;
	float	m_fMid2;
	float	m_fEnd;

	float	*m_values;

public:
	CString	m_StrTitle;

	CString m_StrUse;
	CString m_StrTemp;
	CString m_StrTemp4;

public:
	CSetMultiSlider(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetMultiSlider();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SETMULTISLIDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CMultiSlider m_Slider;
	afx_msg void OnEnChangeEditStart();
	afx_msg void OnEnChangeEditMid1();
	afx_msg void OnEnChangeEditMid2();
	afx_msg void OnEnChangeEditEnd();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckUse();
	afx_msg void OnEnChangeEditTemp();
};
