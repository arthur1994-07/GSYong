#pragma once

#include "afxcmn.h"
#include "../MainFrm.h"
#include "../../RanLogic/GLogicEx.h"
#include "afxwin.h"

namespace gmce
{
	class IDBManager;
}

// CItemTransfer �� ���Դϴ�.

class CItemTransfer : public CFormView
{
	DECLARE_DYNCREATE(CItemTransfer)

protected:
	CItemTransfer();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CItemTransfer();

public:
	enum { IDD = IDD_ITEMTRANSFER };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	HANDLE m_hThread;	
	BOOL m_bInit;	
	BOOL m_bRunning;

	void ItemTransferProc();

	gmce::IDBManager* m_pDBMan;

	int m_nCheck;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnItemTransferStart();
	afx_msg void OnBnClickedBtnItemTransferStop();
	virtual void OnInitialUpdate();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnMaxtextEditItemTransferConsole();
	CProgressCtrl m_Progress;
	CEdit m_EditItemTransferConsole;
	afx_msg void OnBnClickedCheckClubitemTransfer();
};


