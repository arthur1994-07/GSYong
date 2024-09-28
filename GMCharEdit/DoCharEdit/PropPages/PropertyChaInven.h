#pragma once
#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Inventory/GLInventory.h"

#include <vector>

namespace gmce
{
	class IDBManager;
}

class CPropertyChaInven : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropertyChaInven)

public:
	CPropertyChaInven();
	virtual ~CPropertyChaInven();

	enum { IDD = IDD_PROPERTY_CHAR_INVEN };

	gmce::IDBManager *m_pDBMan;

	std::vector<SITEMCUSTOM> m_vecWriteAddItemLogs;
	std::vector<SITEMCUSTOM> m_vecWriteAddRamdomoptionLogs;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
    void SetData(SCHARDATA2* pChar);
    void GetData(SCHARDATA2* pChar);
    void InitListHead();
    
	std::vector<GLInventory::CELL_KEY> m_vInvenKey;
    std::vector<SINVENITEM> m_vInvenItem;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CListCtrl m_ListItem;
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedBtnInvenEdit();
	afx_msg void OnBnClickedBtnInvenAdd();
	afx_msg void OnBnClickedBtnInvenDelete();
	afx_msg void OnBnClickedBtnInvenSave();
	virtual void OnOK();
};
