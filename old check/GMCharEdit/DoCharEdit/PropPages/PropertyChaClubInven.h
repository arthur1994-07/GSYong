#pragma once

#include "afxcmn.h"

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Inventory/GLInventory.h"

#include <vector>

namespace gmce
{
	class IDBManager;
}

class CPropertyChaClubInven : public CPropertyPage
{
	DECLARE_DYNAMIC( CPropertyChaClubInven )

public:
	struct SCLUBINVENITEM
	{
		unsigned int nChannel;
		SINVENITEM sItem;
	};

public:
	CPropertyChaClubInven();
	virtual ~CPropertyChaClubInven();

	enum { IDD = IDD_PROPERTY_CHAR_CLUB_INVEN };

	gmce::IDBManager* m_pDBMan;

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

public:
	void SetData();
	//void GetData(SCHARDATA2* pChar);
	void InitListHead();

	std::vector<GLInventory::CELL_KEY> m_vInvenKey;
	std::vector<SCLUBINVENITEM> m_vInvenItem;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ListItem;
	CEdit m_moneyEditBox;
	LONGLONG m_llMoney;
	//afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnBnClickedButton2();
	//afx_msg void OnBnClickedBtnInvenEdit();
	//afx_msg void OnBnClickedBtnInvenAdd();
	afx_msg void OnBnClickedBtnInvenDelete();
	afx_msg void OnBnClickedBtnInvenSave();
	virtual void OnOK();
};