#pragma once
#include "afxwin.h"

#include "../Static/StProg.h"

#define _TIMER_ID	(0x00000100)

// CEffLiftTimeBar 대화 상자입니다.
struct EFF_PROPERTY;
struct EFF_PROPGROUP;
class CEffLiftTimeBar : public CDialog
{
	DECLARE_DYNAMIC(CEffLiftTimeBar)

protected:
	int		m_nTimer;
	float	m_fScale;

protected:
	BOOL ListingTimeLine ( EFF_PROPERTY* pCurProp );

public:
	BOOL ListingTimeLine ( EFF_PROPGROUP *pEffPropGroup );

public:
	CEffLiftTimeBar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEffLiftTimeBar();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFLIFETIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStProg		m_stEffTimeLine;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CScrollBar m_HScrollBar;
	CScrollBar m_VScrollBar;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDeltaposSpinScale(NMHDR *pNMHDR, LRESULT *pResult);
};
