#pragma once
#include "afxwin.h"
#include "../MfcExLib/ExLibs/CheckListBoxLog.h"


// CCheckIntegrityNaviMeshDlg 대화 상자입니다.

class CCheckIntegrityNaviMeshDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckIntegrityNaviMeshDlg)

public:
	CCheckIntegrityNaviMeshDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCheckIntegrityNaviMeshDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CHECKINTEGRITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CCheckListBoxLog m_ctrlCheckListBoxLog;
	CButton m_ctrlButtonOK;
	CButton m_ctrlButtonIGNORE;
	CString m_StaticCount;
	DWORD	(*m_pCellTable)[10];
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnLbnDblclkListCheckintegritylog();
	CString m_StaticIslandCount;
	CCheckListBoxLog m_ctrlCheckListBoxLog2;
	DWORD **m_ppIslandID;
	DWORD *m_pSameIslandCount;
	DWORD m_nIslandCount;
	afx_msg void OnLbnDblclkListCheckintegritylog2();
	afx_msg void OnBnClickedIgnore();
};
