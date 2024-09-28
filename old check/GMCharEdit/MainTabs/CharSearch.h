#pragma once
#include "afxcmn.h"

#include <vector>
#include <strstream>

#include "../Util/CGmCfg.h"
#include "../Util/CConsoleMessage.h"
#include "../Database/DatabaseTable.h"
#include "../Database/ADO/ADOManager.h"
#include "../Database/ODBC/CGMOdbcManager.h"

#include "afxwin.h"

namespace gmce
{
	class IDBManager;
}

class CCharSearch : public CFormView
{
	DECLARE_DYNCREATE(CCharSearch)

protected:
	CCharSearch();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCharSearch();

public:
	enum { IDD = IDD_CHARSEARCH };

	enum
    { 
        SEARCH_USERID=1,
        SEARCH_CHANAME=2,
        SEARCH_USERNUM=3,
        SEARCH_CHANUM=4
    };

    void SearchUserID(CString strID);
    void SearchChaName(const CString& strChaName);
    void SearchUserNum(int nUserNum);
    void SearchChaNum(int nChaNum);

    void InitListHead();
    void FillData();

	CString m_strSearch;

    int     m_nRadio;    
	BOOL	m_bInit;
	std::vector<CharInfo> m_vChaInfo;

	gmce::IDBManager	*m_pDBMan;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	CListCtrl m_ListSearch;
	CEdit m_EditSearch;
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedRadioChaname();
	afx_msg void OnBnClickedRadioChanum();
	afx_msg void OnBnClickedRadioUsernum();
	afx_msg void OnBnClickedRadioUserid();
	afx_msg void OnLvnItemchangedListSearch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListSearch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListSearch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContext1Makecroncharacter();
	afx_msg void OnNMCustomdrawListSearch(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};