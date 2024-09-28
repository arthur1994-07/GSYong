#pragma once
#include "afxcmn.h"
#include "../MainFrm.h"
#include "../../RanLogic/GLogicEx.h"
#include "afxwin.h"

namespace gmce
{
	class IDBManager;
}

class CItemSearch : public CFormView
{
	DECLARE_DYNCREATE(CItemSearch)

protected:
	CItemSearch();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CItemSearch();

public:
	void InitListHead();
	
	void ItemSearchStart();
	void SpecialItemSearchStart();
	
	void SearchProc();
	///////////////////////////////////////////////////////////////////////////////
	// ����� �������� �˻��ϱ� ���ؼ� �������
	void SpecialSearchProc();

	BOOL   m_bRunning;
	HANDLE m_hThread;
	BOOL   m_bInit;

	gmce::IDBManager *m_pDBMan;

public:
	enum { IDD = IDD_ITEMSEARCH };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCharSearch;
	int m_nItemMain;
	int m_nItemSub;
	CString m_guid;
	int m_nCheck;
	CEdit m_nItemCount;
	int nFindCount;
	CProgressCtrl m_SearchProgress;
	CEdit m_EditSearchConsole;
	virtual void OnInitialUpdate();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnItemSearchStart();
	afx_msg void OnEnMaxtextEditItemSearchConsole();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnSelectItem();
	afx_msg void OnBnClickedBtnItemSearchStop();
	afx_msg void OnBnClickedBtnItemSpecialSearchStart();
	afx_msg void OnClose();		
	afx_msg void OnBnClickedCheck();
};


