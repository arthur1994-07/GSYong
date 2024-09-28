#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"

namespace gmce
{
	class IDBManager;
}

class CPropertyChaUserInven : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyChaUserInven)

public:
	CPropertyChaUserInven();
	virtual ~CPropertyChaUserInven();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPERTY_CHAR_USERINVEN };
public:
	void SetData(SCHARDATA2* pChar);
    void GetData(SCHARDATA2* pChar);
    void InitListHead();

	std::vector<GLInventory::CELL_KEY> m_vInvenKey;
    std::vector<SINVENITEM> m_vInvenItem;
    int m_nUserInvenNum;

	gmce::IDBManager *m_pDBMan;

	std::vector<SITEMCUSTOM> m_vecWriteAddItemLogs;
	std::vector<SITEMCUSTOM> m_vecWriteAddRamdomoptionLogs;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ListUserInven;
	afx_msg void OnBnClickedBtnUserInvenAdd();
	afx_msg void OnBnClickedBtnUserInvenEdit();
	afx_msg void OnBnClickedBtnUserInvenDelete();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnUserInvenSave();
	virtual void OnOK();
	LONGLONG m_llMoney;    
    int m_nInven2Year;
    int m_nInven2Month;
    int m_nInven2Day;
    int m_nInven2Hour;
    int m_nInven2Min;
    int m_nInven3Year;
    int m_nInven3Month;
    int m_nInven3Day;
    int m_nInven3Hour;
    int m_nInven3Min;
    int m_nInven4Year;
    int m_nInven4Month;
    int m_nInven4Day;
    int m_nInven4Hour;
    int m_nInven4Min;


};
