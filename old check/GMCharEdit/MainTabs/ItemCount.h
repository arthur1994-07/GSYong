#pragma once
#include "afxcmn.h"
#include "../MainFrm.h"
#include "../../RanLogic/GLogicEx.h"
#include "afxwin.h"
#include <time.h> 

struct ItemData
{
	int nMainID;
	int nSubID;
	CString ItemName;
	int nCount;
};


namespace gmce
{
	class IDBManager;
}


// CItemCount �� ���Դϴ�.

class CItemCount : public CFormView
{
	DECLARE_DYNCREATE(CItemCount)

protected:
	CItemCount();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CItemCount();

public:
	void SearchProc();		// ������ �˻��� ���� �Լ�
	void InitListHead();	// ListCtrl�� ��� ����
	void InitComboHead();	// ListCtrl�� ��� ����

	void ItemSearchStart();

	BOOL					m_bRunning;
	HANDLE					m_hThread;
	BOOL					m_bInit;

	gmce::IDBManager* m_pDBMan;

public:
	enum { IDD = IDD_ITEMCOUNT };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	void	AddString ( const char* szData ); 

	int			m_nWeaponsCheck;
	int			m_nDefenseCheck;
	int			m_nItemCountCheck;

	virtual void OnInitialUpdate();

	afx_msg void OnBnClickedCheckWeapons();
	afx_msg void OnBnClickedCheckDefense();
	afx_msg void OnBnClickedCheckItemcount();
	afx_msg void OnNMDblclkListWeapons(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnNMDblclkListDefense(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListItemcount(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListCtrlWeapons;
	CListCtrl m_ListCtrlDefense;
	CListCtrl m_ListCtrlCount;
	afx_msg void OnBnClickedButton1();	
	afx_msg void OnBnClickedButtonStop();
	CComboBox m_SYear;
	CComboBox m_SMonth;
	CComboBox m_SDay;
	CComboBox m_EYear;
	CComboBox m_EMonth;
	CComboBox m_EDay;
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeComboYear();
	CString m_nSYear;
	CString m_nSMonth;
	CString m_nSDay;
	CString m_nEYear;
	CString m_nEMonth;
	CString m_nEDay;
	CEdit m_EditCount;
};


