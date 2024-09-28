// ZplayOggExamDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class BgmSound;

// CZplayOggExamDlg dialog
class CZplayOggExamDlg : public CDialog
{
// Construction
public:
	CZplayOggExamDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ZPLAYOGGEXAM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonLoad();
    CStatic m_StaticSoundFile;
    CButton m_Loop;
    CSliderCtrl m_Volume;
    CProgressCtrl m_SoundProgress;
    BgmSound* m_pBgmSound;
    afx_msg void OnClose();
    afx_msg void OnBnClickedButtonPlay();
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnBnClickedButtonPause();
    afx_msg void OnBnClickedButtonResume();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
