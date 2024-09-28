#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "../MainFrm.h"
#include "../../RanLogic/GLogicEx.h"

namespace gmce
{
	class IDBManager;
}

class CSearchDuplicationItems : public CFormView
{
	DECLARE_DYNCREATE(CSearchDuplicationItems)

protected:
	CSearchDuplicationItems();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CSearchDuplicationItems();

public:
	void InitListHead();
	
	void StartSearchItem();	
	void SearchProc();

    void StartSearchCharacter();
    void SearchCharacterProc();

    void StartSearchExchangeItemHistory();
    void SearchExchangeItemHistoryProc();
    void ShowExchangeItemHistory();

	BOOL   m_bRunning;
	HANDLE m_hThread;
	BOOL   m_bInit;

	gmce::IDBManager *m_pDBMan;

public:
	enum { IDD = IDD_SEARCH_DUPLICATION_ITEMS };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCharSearch;
    CListCtrl m_ListCharSearch2;
	int nFindCount;
	CProgressCtrl m_SearchProgress;
	CEdit m_EditSearchConsole;

    std::vector<ExchangeItemLog> ExchangeLog;
    std::vector<ExchangeItemHistory> ExchangeHistory;

    std::vector<int> GenType;
    std::vector<int> ExchangeFlag;

    int m_nResultSortColumn[2];
    bool m_bResultSortDirect[2];

    LONGLONG m_nItemMakeNum;
    LONGLONG m_nItemMakeNum2;

    bool m_bCheckItemID;
    int m_ItemMID;
    int m_ItemSID;

	virtual void OnInitialUpdate();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnMaxtextEditItemSearchConsole();
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedBtnSearchDupStart();
	afx_msg void OnBnClickedBtnSearchDupStop();
	afx_msg void OnClose();
    afx_msg void OnBnClickedBtnMakeType();
    afx_msg void OnBnClickedBtnExchangeFlag();
    afx_msg void OnBnClickedBtnSearchDupStart2();
    afx_msg void OnBnClickedBtnSearchDupStop2();
    afx_msg void OnHdnItemclickSearhDupList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedBtnViewHistory();
    afx_msg void OnBnClickedBtnSearchDupStop3();
    afx_msg void OnBnClickedCheckItemId();
};