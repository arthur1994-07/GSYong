#pragma once
#include "afxwin.h"
#include "../MfcExLib/ExLibs/CheckListBoxLog.h"


// CCheckIntegrityNaviMeshDlg ��ȭ �����Դϴ�.

class CCheckIntegrityNaviMeshDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckIntegrityNaviMeshDlg)

public:
	CCheckIntegrityNaviMeshDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCheckIntegrityNaviMeshDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CHECKINTEGRITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
