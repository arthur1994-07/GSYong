#pragma once

#include "afxcmn.h"
#include "../MainFrm.h"
#include "../../RanLogic/GLogicEx.h"
#include "afxwin.h"

namespace gmce
{
	class IDBManager;
}

class CGMItemSearch : public CFormView
{
	DECLARE_DYNCREATE(CGMItemSearch)

protected:
	CGMItemSearch();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CGMItemSearch();

public:
	enum { IDD = IDD_GMITEMSEARCH };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	HANDLE m_hThread;	
	BOOL m_bInit;	
	BOOL m_bRunning;

	void GMItemSearchProc();
	void InitListHead();

	gmce::IDBManager	*m_pDBMan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnGmitemSearchStart();
	afx_msg void OnBnClickedBtnGmitemSearchStop();
	virtual void OnInitialUpdate();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnMaxtextEditGmitemSearchConsole();
	CProgressCtrl m_Progress;
	CListCtrl m_ListFind;
	afx_msg void OnNMDblclkListGmitemSearch(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_EditGmSearchConsole;
};


