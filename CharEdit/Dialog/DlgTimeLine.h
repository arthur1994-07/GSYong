#if !defined(AFX_DLGTIMELINE_H__9F998182_6E00_426F_8485_E6762C779458__INCLUDED_)
#define AFX_DLGTIMELINE_H__9F998182_6E00_426F_8485_E6762C779458__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTimeLine.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeLine dialog

#include "../Static/STimeLine.h"

class CDlgTimeLine : public CDialog
{
protected:
	CSTimeLine	m_TimeLine;

public:
	void SetTimeLine ( DWORD dwStart, DWORD dwEnd, DWORD dwUnitTime, BYTE aniType = 1 );
	void SetTimeState ( DWORD dwToTime );

	void SetMultiAnimPlay();
	void SetPlay();
	void SetStop();

	BOOL IsTick ()			{ return m_TimeLine.IsTick (); }
	DWORD GetTickTime ()	{ return m_TimeLine.GetTickTime(); }
	void ResetTick ()		{ return m_TimeLine.ResetTick (); }
	void SetFrameMarking ( WORD wMarkCount, WORD* pMarkArray=NULL )	{	m_TimeLine.SetFrameMarking ( wMarkCount, pMarkArray );	}
	float GetAniDelay();

// Construction
public:
	CDlgTimeLine(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTimeLine)
	enum { IDD = IDD_DIALOG_TIMELINE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTimeLine)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTimeLine)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTos();
	afx_msg void OnButtonPkey();
	afx_msg void OnButtonPlay();
	afx_msg void OnButtonNkey();
	afx_msg void OnButtonEkey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTIMELINE_H__9F998182_6E00_426F_8485_E6762C779458__INCLUDED_)
